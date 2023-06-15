// Fill out your copyright notice in the Description page of Project Settings.


#include "BallisticMovementComponent.h"

void UBallisticMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (FMath::IsNearlyZero(arcRadius))
	{
		return;
	}

	if (bMovementFrozen)
	{
		return;
	}

	if (currentSpeed < maxSpeed)
	{
		currentSpeed += acceleration * DeltaTime;
	}

	if (currentSpeed > maxSpeed)
	{
		currentSpeed = maxSpeed;
	}

	if (distanceTraveled >= arcLength)
	{
		Velocity = FVector::DownVector * currentSpeed * DeltaTime;
		UpdateComponentVelocity();
		return;
	}

	FRotator heading = (launchLocation - targetLocation).Rotation();
	
	float arcStep = currentSpeed * DeltaTime;
	distanceTraveled += arcStep;

	float angleRad = distanceTraveled / arcLength * PI;
	float tangentRad = angleRad + PI / 2;
	float tangentDeg = FMath::RadiansToDegrees(tangentRad);
	heading.Pitch = tangentDeg;

	GetOwner()->SetActorRotation(heading);

	FVector direction = (targetLocation - launchLocation) / totalDistance;
	float xyShift = (-FMath::Cos(angleRad) + 1) / 2 * totalDistance;
	FVector arcLocation = launchLocation + xyShift * direction;

	arcLocation.Z = FMath::Sin(angleRad) * arcRadius;
	FVector locationStep = arcLocation - GetOwner()->GetActorLocation();
	Velocity = locationStep;
	UpdateComponentVelocity();

	GetOwner()->SetActorLocation(arcLocation);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBallisticMovementComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UBallisticMovementComponent::SetTargetAndLaunchLocation(const FVector& target, const FVector& launch)
{
	targetLocation = target;
	launchLocation = launch;

	totalDistance = (targetLocation - launchLocation).Size();
	float diameter = (targetLocation - launchLocation).Size2D();
	arcRadius = diameter / 2;

	arcLength = PI * diameter / 2;

	float launchZ = launch.Z;
	float launchAngle = FMath::FastAsin(launchZ / arcRadius);
	distanceTraveled = diameter * launchAngle;
}

void UBallisticMovementComponent::FreezeMovement()
{
	bMovementFrozen = true;
}

bool UBallisticMovementComponent::PredictLocation(float time, FVector& outLocation) const
{
	check(time >= 0);
	if (FMath::IsNearlyZero(time))
	{
		outLocation = GetOwner()->GetActorLocation();
		return true;
	}

	if (bMovementFrozen)
	{
		outLocation = GetOwner()->GetActorLocation();
		return true;
	}

	float distancePrediction = distanceTraveled;
	if (FMath::IsNearlyEqual(currentSpeed, maxSpeed))
	{
		distancePrediction += maxSpeed * time;
	}
	else if(currentSpeed < maxSpeed)
	{
		float speedDelta = maxSpeed - currentSpeed;
		float untilMaxSpeed = speedDelta / acceleration;
		distancePrediction += + currentSpeed * untilMaxSpeed + acceleration * untilMaxSpeed * untilMaxSpeed / 2;
		distancePrediction += maxSpeed * (time - untilMaxSpeed);
	}
	else
	{
		// Movement faster then maxSpeed should be impossible!
		return false;
	}

	if (distancePrediction >= arcLength)
	{
		outLocation = targetLocation;
		return true;
	}

	float angleRad = distancePrediction / arcLength * PI;
	FVector direction = (targetLocation - launchLocation) / totalDistance;
	float xyShift = (-FMath::Cos(angleRad) + 1) / 2 * totalDistance;
	FVector arcLocation = launchLocation + xyShift * direction;
	arcLocation.Z = FMath::Sin(angleRad) * arcRadius;
	outLocation = arcLocation;
	return true;
}

FVector UBallisticMovementComponent::GetTargetLocation() const
{
	return targetLocation;
}
