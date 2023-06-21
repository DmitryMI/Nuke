// Fill out your copyright notice in the Description page of Project Settings.


#include "AIUtils.h"
#include "Attackable.h"
#include "Radar.h"
#include "Kismet/GameplayStatics.h"
#include "RadarDetectorComponent.h"

bool UAIUtils::AreEnemies(AActor* actor1, AActor* actor2)
{
	IGenericTeamAgentInterface* teamAgent1 = Cast<IGenericTeamAgentInterface>(actor1);
	IGenericTeamAgentInterface* teamAgent2 = Cast<IGenericTeamAgentInterface>(actor2);

	if (!teamAgent1 || !teamAgent2)
	{
		return false;
	}

	return FGenericTeamId::GetAttitude(actor1, actor2) == ETeamAttitude::Hostile;
}

bool UAIUtils::CalculateInterceptionLinear(
	const FVector& targetLocation,
	const FVector& targetVelocity,
	const FVector& launchLocation,
	double projectileSpeed,
	FVector& outInterceptionLocation)
{

	// Based on https://discussions.unity.com/t/how-to-calculate-the-point-of-intercept-in-3d-space/22540/2
	float t;
	bool ok = CalculateInterceptionLinearTime(
		targetLocation - launchLocation,
		targetVelocity, projectileSpeed, t);
	if (!ok)
	{
		return false;
	}
	outInterceptionLocation = targetLocation + t * (targetVelocity);
	return true;
}

bool UAIUtils::CalculateInterceptionLinearTime(const FVector& targetRelativePosition, const FVector& targetVelocity, float shotSpeed, float& outTime)
{
	float velocitySquared = targetVelocity.SizeSquared();
	if (velocitySquared < 0.001f)
	{
		outTime = 0.0f;
		return true;
	}

	float a = velocitySquared - shotSpeed * shotSpeed;

	//handle similar velocities
	if (FMath::Abs(a) < 0.001f)
	{
		float t = -targetRelativePosition.SizeSquared()	/ (2.0f * FVector::DotProduct(targetVelocity,targetRelativePosition));
		outTime = t;
		return t >= 0; //don't shoot back in time
	}

	float b = 2.0f * FVector::DotProduct(targetVelocity, targetRelativePosition);
	float c = targetRelativePosition.SizeSquared();
	float determinant = b * b - 4.0f * a * c;

	if (determinant > 0.0f)
	{
		float t1 = (-b + FMath::Sqrt(determinant)) / (2.0f * a);
		float t2 = (-b - FMath::Sqrt(determinant)) / (2.0f * a);
		if (t1 > 0.0f)
		{
			if (t2 > 0.0f)
			{
				outTime = FMath::Min(t1, t2);
				return true;
			}
			else
			{
				outTime = t1;
				return true;
			}
		}
		else
		{
			outTime = t2;
			return t2 >= 0; //don't shoot back in time
		}
	}
	else if (determinant < 0.0f) //determinant < 0; no intercept path
	{
		return false;
	}
	else //determinant = 0; one intercept path, pretty much never happens
	{
		outTime = -b / (2.0f * a);
		return outTime >= 0; //don't shoot back in time
	}

}

int UAIUtils::QuadraticEquation(double a, double b, double c, double& x1, double& x2)
{
	double determinant = b * b - 4.0f * a * c;
	if (determinant < 0)
	{
		return 0;
	}
	else if (determinant > 0)
	{
		double detSq = FMath::Sqrt(determinant);
		x1 = (-b + FMath::Sqrt(determinant)) / (2.0f * a);
		x2 = (-b - FMath::Sqrt(determinant)) / (2.0f * a);
		return 2;
	}
	else
	{
		x1 = -b / (2.0f * a);
		return 1;
	}

}

bool UAIUtils::CalculateInterceptionBallistic(UBallisticMovementComponent* targetMovement, const FVector& launchLocation, double projectileInitialSpeed, double projectileAcceleration, double projectileMaxSpeed, FVector& outInterceptionLocation, double& outInterceptionTime, double& launchDelay)
{
	FVector targetDestination = targetMovement->GetTargetLocation();
	double estimatedTimeToTarget = (launchLocation - targetMovement->GetOwner()->GetActorLocation()).Size() / projectileMaxSpeed;

	double timeToFullSpeed = (projectileMaxSpeed - projectileInitialSpeed) / projectileAcceleration;

	double timeStep = estimatedTimeToTarget / 100;
	double predictionTimeShift = 0;
	while (predictionTimeShift < estimatedTimeToTarget)
	{
		FVector predictedLocation;
		bool predictionOk = targetMovement->PredictLocation(predictionTimeShift, predictedLocation);
		if (!predictionOk)
		{
			return false;
		}
		
		if ((predictedLocation - targetDestination).IsNearlyZero())
		{
			return false;
		}

		double distance = (launchLocation - predictedLocation).Size();
		double acceleratingDistance = projectileInitialSpeed * timeToFullSpeed + projectileAcceleration * timeToFullSpeed * timeToFullSpeed / 2;
		if (distance < acceleratingDistance)
		{
			double travelTime;
			double travelTime1;
			double travelTime2;
			int travelTimeRoots = QuadraticEquation(projectileAcceleration / 2, projectileInitialSpeed, -distance, travelTime1, travelTime2);
			if (travelTimeRoots == 0)
			{
				return false;
			}
			else if (travelTimeRoots == 1)
			{
				if (travelTime1 < 0)
				{
					travelTime = travelTime2;
				}
				else if (travelTime2 < 0)
				{
					travelTime = travelTime1;
				}
				else
				{
					travelTime = FMath::Min(travelTime1, travelTime2);
				}
			}
			else
			{
				travelTime = travelTime1;
			}

			if (travelTime < predictionTimeShift)
			{
				outInterceptionLocation = predictedLocation;
				launchDelay = predictionTimeShift - travelTime;
				outInterceptionTime = predictionTimeShift;
				return true;
			}
		}
		else
		{
			double linearDistance = distance - acceleratingDistance;
			double linearTravelTime = linearDistance / projectileMaxSpeed;
			double travelTime = linearTravelTime + timeToFullSpeed;
			if (travelTime < predictionTimeShift)
			{
				outInterceptionLocation = predictedLocation;
				launchDelay = predictionTimeShift - travelTime;
				outInterceptionTime = predictionTimeShift;
				return true;
			}
		}
		predictionTimeShift += timeStep;
	}

	return false;
}

bool UAIUtils::IsAttackableAlive(AActor* attackableActor)
{
	IAttackable* attackable = Cast<IAttackable>(attackableActor);
	if (!attackable)
	{
		return false;
	}

	return attackable->IsAlive();
}

void UAIUtils::AttackableReceiveDamage(AActor* attackableActor, float damageAmount)
{
	IAttackable* attackable = Cast<IAttackable>(attackableActor);
	if (!attackable)
	{
		return;
	}

	attackable->ReceiveDamage(damageAmount);
}

AActor* UAIUtils::GetClosestActor(const FVector& fromLocation, TArray<AActor*> actors)
{
	if (actors.Num() == 0)
	{
		return nullptr;
	}

	float minDistance = (actors[0]->GetActorLocation() - fromLocation).SizeSquared();
	AActor* closestActor = actors[0];

	for (AActor* actor : actors)
	{
		float distance = (actor->GetActorLocation() - fromLocation).SizeSquared();
		if (distance < minDistance)
		{
			closestActor = actor;
			minDistance = distance;
		}
	}

	return closestActor;
}

TArray<FAircraftWaypoint> UAIUtils::GenerateRandomAircraftPath(FVector startingLocation, float segmentLength, int points, FVector2D altitudeRange)
{
	TArray<FAircraftWaypoint> result;

	FVector2D randomPoint2D = FMath::RandPointInCircle(segmentLength);
	FVector randomPoint = FVector(randomPoint2D.X, randomPoint2D.Y, 0);
	FAircraftWaypoint lastWaypoint;
	lastWaypoint.Location = startingLocation + randomPoint;
	lastWaypoint.Location.Z = FMath::RandRange(altitudeRange.X, altitudeRange.Y);
	lastWaypoint.Speed = 1500.0f;

	result.Add(lastWaypoint);
	
	for (int i = 0; i < points; i++)
	{
		randomPoint2D = FMath::RandPointInCircle(segmentLength);
		randomPoint = FVector(randomPoint2D.X, randomPoint2D.Y, 0);
		lastWaypoint.Location = lastWaypoint.Location + randomPoint;
		lastWaypoint.Location.Z = FMath::RandRange(altitudeRange.X, altitudeRange.Y);
		result.Add(lastWaypoint);
	}

	return result;
}

TArray<AActor*> UAIUtils::GetEnemyCities(AActor* teamAgentActor)
{
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(teamAgentActor);

	TArray<AActor*> cities;
	TArray<AActor*> cityActors;
	UGameplayStatics::GetAllActorsOfClass(teamAgentActor, ACity::StaticClass(), cityActors);

	for (AActor* actor : cityActors)
	{
		ACity* city = Cast<ACity>(actor);
		if (teamAgent->GetTeamAttitudeTowards(*city) == ETeamAttitude::Hostile)
		{
			cities.Add(city);
		}
	}

	return cities;
}

AActor* UAIUtils::GetRandomActor(const TArray<AActor*>& actors)
{
	if (actors.Num() == 0)
	{
		return nullptr;
	}

	int index = FMath::RandRange(0, actors.Num() - 1);
	return actors[index];
}

bool UAIUtils::IsActorTrackedByRadar(AActor* radarActor, AActor* trackableActor)
{
	IRadar* radar = Cast<IRadar>(radarActor);
	return radar->IsActorTrackedByRadar(trackableActor);
}

bool UAIUtils::GetTrackedThreatsByRadar(AActor* radarActor, TArray<AActor*>& outTrackedThreats)
{
	IRadar* radar = Cast<IRadar>(radarActor);
	if (!radar)
	{
		return false;
	}
	return radar->GetTrackedThreats(outTrackedThreats);
}

bool UAIUtils::GetTrackingMissiles(AActor* targetActor, TArray<AMissile*>& outMissiles)
{
	URadarDetectorComponent* detector = targetActor->GetComponentByClass<URadarDetectorComponent>();
	if (!detector)
	{
		return false;
	}

	return detector->GetDetectedRadars(outMissiles);
}
