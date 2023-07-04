// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipMovementComponent.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"

void UShipMovementComponent::AnimateShipBuoyancy(float DeltaTime, float& zOffset, float& rollOffset)
{
	float sinArg0 = GetWorld()->GetTimeSeconds() * rollTiltSpeed;
	rollOffset = rollTiltAngleMax * FMath::Sin(sinArg0);

	float sinArg1 = GetWorld()->GetTimeSeconds() * bodyZSwingSpeed;
	zOffset = bodyZSwingMax * FMath::Sin(sinArg1);
}

void UShipMovementComponent::GetVelocityAndRotationFromInput(float DeltaTime, FVector& nextVelocity, FRotator& nextRotation)
{
	FVector inputVector = ConsumeInputVector();
	
	float targetSpeed = inputVector.Size() * maxSpeed;
	targetSpeed = FMath::Clamp(targetSpeed, 0, maxSpeed);

	if(targetSpeed > currentSpeed)
	{
		currentSpeed += acceleration * DeltaTime;
		if (currentSpeed > targetSpeed)
		{
			currentSpeed = targetSpeed;
		}
	}
	else if(targetSpeed < currentSpeed)
	{
		currentSpeed -= deceleration * DeltaTime;
		if (currentSpeed < targetSpeed)
		{
			currentSpeed = targetSpeed;
		}
	}

	nextRotation = GetOwner()->GetActorRotation();

	if (inputVector.IsNearlyZero())
	{
		nextVelocity = Velocity.GetSafeNormal(0.01) * currentSpeed * DeltaTime;
	}
	else
	{
		FRotator rotationToTarget = inputVector.Rotation();
		double yawDelta = FMath::FindDeltaAngleDegrees(GetOwner()->GetActorRotation().Yaw, rotationToTarget.Yaw);
		double yawStep = yawDelta;
		if (FMath::Abs(yawDelta) > maxAngularSpeed * DeltaTime)
		{
			yawStep = FMath::Sign(yawDelta) * maxAngularSpeed * DeltaTime;
		}
		nextRotation.Yaw += yawStep;
		nextVelocity = GetOwner()->GetActorForwardVector() * currentSpeed * DeltaTime;
	}
}

void UShipMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent);

	if (primitiveComponent->GetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic) == ECollisionResponse::ECR_Ignore)
	{
		primitiveComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	}
	primitiveComponent->SetGenerateOverlapEvents(true);

	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	auto supportedAgents = navSystem->GetSupportedAgents();
	FNavDataConfig firstAgent = supportedAgents[0];
	NavAgentProps = firstAgent;
	//NavAgentProps.SetPreferredNavData(ARecastNavMesh::StaticClass());
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

UShipMovementComponent::UShipMovementComponent()
{
	bUseAccelerationForPaths = true;
}

void UShipMovementComponent::RequestPathMove(const FVector& MoveInput)
{
	Super::RequestPathMove(MoveInput);
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

	float buoyancyZOffset;
	float buoyancyRollOffset;
	AnimateShipBuoyancy(DeltaTime, buoyancyZOffset, buoyancyRollOffset);

	FVector location = owner->GetActorLocation();
	location.Z = waterSurfaceZ + bodyZOffset + buoyancyZOffset;

	FVector nextVelocity;
	FRotator nextRotation;
	GetVelocityAndRotationFromInput(DeltaTime, nextVelocity, nextRotation);
	nextRotation.Roll = buoyancyRollOffset;

	Velocity = nextVelocity;
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

	owner->SetActorLocationAndRotation(location, nextRotation);
}
