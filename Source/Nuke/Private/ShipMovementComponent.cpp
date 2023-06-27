// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipMovementComponent.h"

void UShipMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent);

	if (primitiveComponent->GetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic) == ECollisionResponse::ECR_Ignore)
	{
		primitiveComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	}
	primitiveComponent->SetGenerateOverlapEvents(true);
}

bool UShipMovementComponent::FindWaterUnderShip(float& waterSurfaceZ, float& waterDepth) const
{
	return TestLocation(GetOwner()->GetActorLocation(), waterSurfaceZ, waterDepth);
}

bool UShipMovementComponent::TestLocation(const FVector& location, float& waterSurfaceZ, float& waterDepth) const
{
	FVector traceStart = location + FVector::UpVector * 1000.0f;
	FVector traceEnd = location + FVector::DownVector * 1000.0f;
	FHitResult hitResult;
	FCollisionQueryParams params;
	bool hasHit = GetWorld()->LineTraceSingleByProfile(hitResult, traceStart, traceEnd, "WaterBodyCollision", params);
	if (!hasHit || hitResult.GetComponent()->GetCollisionProfileName() != "WaterBodyCollision")
	{
		return false;
	}
	waterSurfaceZ = hitResult.ImpactPoint.Z;

	FHitResult seabottomHitResult;
	traceStart.Z = hitResult.ImpactPoint.Z;
	traceEnd.Z = hitResult.ImpactPoint.Z - 10000.0f;
	FCollisionQueryParams seabottomParams;
	AActor* waterActor = hitResult.GetActor();
	seabottomParams.AddIgnoredActor(waterActor);
	bool seabedHasHit = GetWorld()->LineTraceSingleByProfile(seabottomHitResult, traceStart, traceEnd, "BlockAll", seabottomParams);
	if (seabedHasHit)
	{
		waterDepth = waterSurfaceZ - seabottomHitResult.ImpactPoint.Z;
	}
	else
	{
		waterDepth = 10000.0f;
	}
	
	return true;
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* owner = GetOwner();

	float waterSurfaceZ;
	float waterDepth;
	bool isOnWater = FindWaterUnderShip(waterSurfaceZ, waterDepth);
	if (!isOnWater)
	{
		return;
	}

	float sinArg0 = GetWorld()->GetTimeSeconds() * rollTiltSpeed;
	rollTilt = rollTiltAngleMax * FMath::Sin(sinArg0);

	float sinArg1 = GetWorld()->GetTimeSeconds() * bodyZSwingSpeed;
	bodyZSwing = bodyZSwingMax * FMath::Sin(sinArg1);

	FVector location = owner->GetActorLocation();
	location.Z = waterSurfaceZ + bodyZOffset + bodyZSwing;

	FRotator rotation = owner->GetActorRotation();
	rotation.Roll = rollTilt;

	FVector direction = targetLocation - owner->GetActorLocation();
	if (direction.SizeSquared() < FMath::Square(targetLocationTolerance))
	{
		currentSpeed -= deceleration * DeltaTime;
		if (currentSpeed < 0)
		{
			currentSpeed = 0;
		}
		Velocity = Velocity.GetSafeNormal(0.01) * currentSpeed * DeltaTime;
	}
	else
	{
		double distanceToTarget = direction.Size();
		double timeToTarget = distanceToTarget / maxSpeed;
		double targetYaw = direction.Rotation().Yaw;
		double yawDelta = FMath::FindDeltaAngleDegrees(owner->GetActorRotation().Yaw, targetYaw);
		double rotationTime = yawDelta / maxAngularSpeed;

		double yawStep = FMath::Min(yawDelta, maxAngularSpeed * DeltaTime);
		rotation.Yaw += yawStep;

		if (rotationTime < timeToTarget)
		{
			currentSpeed += acceleration * DeltaTime;
			if (currentSpeed > maxSpeed)
			{
				currentSpeed = maxSpeed;
			}

			Velocity = owner->GetActorForwardVector() * currentSpeed * DeltaTime;
		}
	}

	FVector nextLocation = location + Velocity;

	float nextWaterSurfaceZ;
	float nextWaterDepth;
	if (TestLocation(nextLocation, nextWaterSurfaceZ, nextWaterDepth) && nextWaterDepth >= minimalDepth)
	{
		location = nextLocation;
	}
	else
	{
		Velocity = FVector::ZeroVector;
	}

	UpdateComponentVelocity();

	owner->SetActorLocationAndRotation(location, rotation);
}
