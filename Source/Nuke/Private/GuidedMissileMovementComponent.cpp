// Fill out your copyright notice in the Description page of Project Settings.


#include "GuidedMissileMovementComponent.h"

void UGuidedMissileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bIsMovementFrozen)
	{
		return;
	}

	if (targetActor == nullptr)
	{
		return;
	}

	if (bIsEngineOn)
	{
		RotateTowardsTarget(DeltaTime);
		if (currentSpeed < maxSpeed)
		{
			currentSpeed += acceleration * DeltaTime;
		}

		if (currentSpeed > maxSpeed)
		{
			currentSpeed = maxSpeed;
		}

		Velocity = GetOwner()->GetActorForwardVector() * currentSpeed * DeltaTime;
	}
	else
	{
		Velocity *= (1 - drag);
		if (Velocity.SizeSquared() < maxSpeed * maxSpeed * DeltaTime)
		{
			Velocity.Z += -100 * DeltaTime;
		}

		FRotator heading = Velocity.Rotation();
		GetOwner()->SetActorRotation(heading);
	}

	
	FVector location = GetOwner()->GetActorLocation();
	location += Velocity;
	GetOwner()->SetActorLocation(location);

	UpdateComponentVelocity();

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGuidedMissileMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	targetLocation = GetOwner()->GetActorForwardVector() * 10000.0f;
}

void UGuidedMissileMovementComponent::RotateTowardsTarget(float DeltaTime)
{
	FVector headingLocation;
	if (bUseTargetActorLocation)
	{
		headingLocation = targetActor->GetActorLocation();
	}
	else
	{
		headingLocation = targetLocation;
	}

	FVector headingVector = headingLocation - GetOwner()->GetActorLocation();
	FRotator targetRotation = headingVector.Rotation();

	FRotator rotation = GetOwner()->GetActorRotation();
	float pitchDelta = FMath::FindDeltaAngleDegrees(rotation.Pitch, targetRotation.Pitch);
	float yawDelta = FMath::FindDeltaAngleDegrees(rotation.Yaw, targetRotation.Yaw);

	float maxTurnStep = maxTurnRateDegrees * DeltaTime;
	float pitchStep;
	if (FMath::Abs(pitchDelta) > FMath::Abs(maxTurnStep))
	{
		pitchStep = FMath::Sign(pitchDelta) * maxTurnStep;
	}
	else
	{
		pitchStep = pitchDelta;
	}

	float yawStep;
	if (FMath::Abs(yawDelta) > FMath::Abs(maxTurnStep))
	{
		yawStep = FMath::Sign(yawDelta) * maxTurnStep;
	}
	else
	{
		yawStep = yawDelta;
	}

	FRotator rotatorChange = FRotator(pitchStep, yawStep, 0);
	rotation += rotatorChange;
	GetOwner()->SetActorRotation(rotation);
}

void UGuidedMissileMovementComponent::SetIsEngineOn(bool isOn)
{
	bIsEngineOn = isOn;
}

AActor* UGuidedMissileMovementComponent::GetTargetActor() const
{
	return targetActor;
}

void UGuidedMissileMovementComponent::SetTargetActor(AActor* actor)
{
	targetActor = actor;
}

void UGuidedMissileMovementComponent::RequestMovementTowardsTargetActor()
{
	bUseTargetActorLocation = true;
}

void UGuidedMissileMovementComponent::RequestMovementTowardsLocation(const FVector& location)
{
	bUseTargetActorLocation = false;
	targetLocation = location;
}

void UGuidedMissileMovementComponent::FreezeMovement()
{
	bIsMovementFrozen = true;
}

float UGuidedMissileMovementComponent::GetMaxSpeed() const
{
	return maxSpeed;
}

float UGuidedMissileMovementComponent::GetAcceleration() const
{
	return acceleration;
}
