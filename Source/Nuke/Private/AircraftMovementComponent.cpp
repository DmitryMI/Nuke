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
		rotation.Roll = 0;
	}
	else if (yawAngleDelta > 0)
	{
		rotation.Yaw += FMath::Min(maxAngularSpeedsPitchYaw.Y * DeltaTime, yawAngleDelta);
		rotation.Roll = 60;
	}
	else
	{
		rotation.Yaw += FMath::Max(-maxAngularSpeedsPitchYaw.Y * DeltaTime, yawAngleDelta);
		rotation.Roll = -60;
	}
	
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
		FVector direction = requestedWaypoint.Location - GetOwner()->GetActorLocation();
		if (!direction.IsNearlyZero())
		{
			FRotator rotatorTowardsWaypoint = direction.Rotation();
			requestedRotation = rotatorTowardsWaypoint;
		}
		else
		{
			requestedRotation = GetOwner()->GetActorRotation();
		}

		requestedRotation.Pitch = FMath::Clamp(requestedRotation.Pitch, pitchMinMax.X, pitchMinMax.Y);

		requestedSpeed = requestedWaypoint.Speed;
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
