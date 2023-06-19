// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftMovementComponent.h"

void UAircraftMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	requestedRotation = GetOwner()->GetActorRotation();
}

void UAircraftMovementComponent::RequestRotation(float pitch, float yaw)
{
	requestedRotation = FRotator(pitch, yaw, 0);
	requestedRotation.Pitch = FMath::Clamp(requestedRotation.Pitch, pitchMinMax.X, pitchMinMax.Y);

	bNavigateToWaypoint = false;
}

void UAircraftMovementComponent::RequestSpeed(float speed)
{
	requestedSpeed = speed;

	if (requestedSpeed > speedMinMax.Y)
	{
		requestedSpeed = speedMinMax.Y;
	}
	else if (requestedSpeed < speedMinMax.X)
	{
requestedSpeed = speedMinMax.X;
	}

	bNavigateToWaypoint = false;
}

void UAircraftMovementComponent::UpdateMovementSpeed(float DeltaTime)
{
	float deltaSpeed;
	float deltaRequestedSpeed = requestedSpeed - currentSpeed;
	if (deltaRequestedSpeed > 0)
	{
		deltaSpeed = ClampAbs(deltaRequestedSpeed, acceleration * DeltaTime);
	}
	else
	{
		deltaSpeed = ClampAbs(deltaRequestedSpeed, deceleration * DeltaTime);
	}
	currentSpeed += deltaSpeed;
	currentSpeed = FMath::Clamp(currentSpeed, speedMinMax.X, speedMinMax.Y);
}

void UAircraftMovementComponent::UpdateActorRotation(const FRotator& toRotation, float DeltaTime)
{
	FRotator rotation = GetOwner()->GetActorRotation();
	double deltaPitch = FMath::FindDeltaAngleDegrees(rotation.Pitch, toRotation.Pitch);
	double deltaYaw = FMath::FindDeltaAngleDegrees(rotation.Yaw, toRotation.Yaw);
	double deltaRoll = FMath::FindDeltaAngleDegrees(rotation.Roll, toRotation.Roll);

	deltaPitch = ClampAbs(deltaPitch, maxVisualRotationSpeed.X * DeltaTime);
	deltaYaw = ClampAbs(deltaYaw, maxVisualRotationSpeed.Y * DeltaTime);
	deltaRoll = ClampAbs(deltaRoll, maxVisualRotationSpeed.Z * DeltaTime);
	FRotator nextRotation = FRotator(rotation.Pitch + deltaPitch, rotation.Yaw + deltaYaw, rotation.Roll + deltaRoll);
	GetOwner()->SetActorRotation(nextRotation);
}

FRotator UAircraftMovementComponent::RotateTowardsTarget(float DeltaTime)
{
	FRotator rotation = Velocity.Rotation();
	float pitchAngleDelta = FMath::FindDeltaAngleDegrees(rotation.Pitch, requestedRotation.Pitch);

	if (pitchAngleDelta > 0)
	{
		rotation.Pitch += FMath::Min(maxAngularSpeedsPitchYaw.X * DeltaTime, pitchAngleDelta);
	}
	else if (pitchAngleDelta < 0)
	{
		rotation.Pitch += FMath::Max(-maxAngularSpeedsPitchYaw.X * DeltaTime, pitchAngleDelta);
	}

	float yawAngleDelta = FMath::FindDeltaAngleDegrees(rotation.Yaw, requestedRotation.Yaw);

	if (FMath::IsNearlyZero(yawAngleDelta))
	{
		yawAngleDelta = 0;
	}
	else if (yawAngleDelta > 0)
	{
		yawAngleDelta = FMath::Min(maxAngularSpeedsPitchYaw.Y * DeltaTime, yawAngleDelta);
	}
	else
	{
		yawAngleDelta = FMath::Max(-maxAngularSpeedsPitchYaw.Y * DeltaTime, yawAngleDelta);
	}
	rotation.Yaw += yawAngleDelta;

	rotation.Roll = maxRollAngleDeg * yawAngleDelta / (maxAngularSpeedsPitchYaw.Y * DeltaTime);

	return rotation;
}

void UAircraftMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bIsMovementFrozen)
	{
		return;
	}

	if (bNavigateToWaypoint)
	{
		requestedSpeed = requestedWaypoint.Speed;

		FVector direction = requestedWaypoint.Location - GetOwner()->GetActorLocation();
		if (!direction.IsNearlyZero())
		{
			float distance = direction.Size();

			FRotator velocityRotator = Velocity.Rotation();
			FRotator rotatorTowardsWaypoint = direction.Rotation();
			if (FMath::IsNearlyZero(rotatorTowardsWaypoint.Yaw))
			{
				requestedRotation.Yaw = 0;
			}
			else
			{
				float yawDelta = FMath::FindDeltaAngleDegrees(velocityRotator.Yaw, rotatorTowardsWaypoint.Yaw);
				float yawDeltaRad = FMath::DegreesToRadians(yawDelta);
				float yawDeltaRadAbs = FMath::Abs(yawDeltaRad);
				float arcDistance = 1 / (2 * FMath::Sin(yawDeltaRadAbs / 2)) * distance * yawDeltaRadAbs;
				float arcFraction = currentSpeed / arcDistance * DeltaTime;
				float yawStep = yawDelta * arcFraction;

				if (FMath::Abs(yawStep) > maxAngularSpeedsPitchYaw.Y * DeltaTime)
				{
					
					if (FMath::IsNearlyEqual(currentSpeed, speedMinMax.X))
					{
						UE_LOG(LogTemp, Warning, TEXT("%s: Nav point %s is unreachable due to yaw turn-rate limits! Slowing down will not help, already at minimal speed!"), *GetOwner()->GetName(), *requestedWaypoint.Location.ToString());
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("%s: Nav point %s is unreachable due to yaw turn-rate limits! Slowing down, maybe this helps"), *GetOwner()->GetName(), *requestedWaypoint.Location.ToString());
						requestedSpeed = speedMinMax.X;
					}
				}

				float minAngularStep = minAngularSpeedsPitchYaw.Y * DeltaTime;
				if (FMath::Abs(yawStep) < minAngularStep)
				{
					float yawStepAbs = FMath::Min(FMath::Abs(yawDelta), minAngularStep);
					yawStep = FMath::Sign(yawStep) * minAngularStep;
				}				
				requestedRotation.Yaw = velocityRotator.Yaw + yawStep;
			}

			if (FMath::IsNearlyZero(rotatorTowardsWaypoint.Pitch))
			{
				requestedRotation.Pitch = 0;
			}
			else
			{
				float pitchDelta = FMath::FindDeltaAngleDegrees(velocityRotator.Pitch, rotatorTowardsWaypoint.Pitch);
				float pitchDeltaRad = FMath::DegreesToRadians(pitchDelta);
				float pitchDeltaRadAbs = FMath::Abs(pitchDeltaRad);
				float arcDistance = 1 / (2 * FMath::Sin(pitchDeltaRadAbs / 2)) * distance * pitchDeltaRadAbs;
				float arcFraction = currentSpeed / arcDistance * DeltaTime;
				float pitchStep = pitchDelta * arcFraction;

				if (FMath::Abs(pitchStep) > maxAngularSpeedsPitchYaw.X * DeltaTime)
				{

					if (FMath::IsNearlyEqual(currentSpeed, speedMinMax.X))
					{
						UE_LOG(LogTemp, Warning, TEXT("%s: Nav point %s is unreachable due to yaw turn-rate limits! Slowing down will not help, already at minimal speed!"), *GetOwner()->GetName(), *requestedWaypoint.Location.ToString());
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("%s: Nav point %s is unreachable due to yaw turn-rate limits! Slowing down, maybe this helps"), *GetOwner()->GetName(), *requestedWaypoint.Location.ToString());
						requestedSpeed = speedMinMax.X;
					}
				}

				float minAngularStep = minAngularSpeedsPitchYaw.X * DeltaTime;
				if (FMath::Abs(pitchStep) < minAngularStep)
				{
					float pitchStepAbs = FMath::Min(FMath::Abs(pitchDelta), minAngularStep);
					pitchStep = FMath::Sign(pitchStep) * minAngularStep;
				}
				requestedRotation.Pitch = velocityRotator.Pitch + pitchStep;
			}

			//requestedRotation = rotatorTowardsWaypoint;
		}
		else
		{
			requestedRotation = GetOwner()->GetActorRotation();
		}

		requestedRotation.Pitch = FMath::Clamp(requestedRotation.Pitch, pitchMinMax.X, pitchMinMax.Y);
	}

	FRotator rotation = RotateTowardsTarget(DeltaTime);
	UpdateActorRotation(rotation, DeltaTime);
	UpdateMovementSpeed(DeltaTime);

	rotation.Roll = 0;
	Velocity = rotation.Vector() * currentSpeed * DeltaTime;

	FVector location = GetOwner()->GetActorLocation();
	location += Velocity;
	GetOwner()->SetActorLocation(location);
	UpdateComponentVelocity();
}

void UAircraftMovementComponent::RequestFlyTowardsWaypoint(const FAircraftWaypoint& waypoint)
{
	bNavigateToWaypoint = true;
	requestedWaypoint = waypoint;
}

bool UAircraftMovementComponent::GetRequestedWaypoint(FAircraftWaypoint& outWaypoint) const
{
	outWaypoint = requestedWaypoint;
	return bNavigateToWaypoint;
}

float UAircraftMovementComponent::GetCurrentSpeed() const
{
	return currentSpeed;
}

void UAircraftMovementComponent::FreezeMovement()
{
	bIsMovementFrozen = true;
}

float UAircraftMovementComponent::GetMaxSpeed() const
{
	return speedMinMax.Y;
}
