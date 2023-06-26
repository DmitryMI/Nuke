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

bool UShipMovementComponent::FindWaterUnderShip(FVector& queryHit) const
{
	FVector traceStart = GetOwner()->GetActorLocation() + FVector::UpVector * 1000.0f;
	FVector traceEnd = GetOwner()->GetActorLocation() + FVector::DownVector * 1000.0f;
	//TArray<FHitResult> hitResults;
	FHitResult hitResult;
	FCollisionQueryParams params;
	bool hasHit = GetWorld()->LineTraceSingleByProfile(hitResult, traceStart, traceEnd, "WaterBodyCollision", params);
	if (!hasHit || hitResult.GetComponent()->GetCollisionProfileName() != "WaterBodyCollision")
	{
		UE_LOG(LogTemp, Error, TEXT("Ship %s is not on water body!"), *GetOwner()->GetName());
		return false;
	}

	queryHit = hitResult.ImpactPoint;
	return true;
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* owner = GetOwner();

	FVector waterSurface;
	bool isOnWater = FindWaterUnderShip(waterSurface);
	if (!isOnWater)
	{
		return;
	}

	float sinArg0 = GetWorld()->GetTimeSeconds() * rollTiltSpeed;
	rollTilt = rollTiltAngleMax * FMath::Sin(sinArg0);

	float sinArg1 = GetWorld()->GetTimeSeconds() * bodyZSwingSpeed;
	bodyZSwing = bodyZSwingMax * FMath::Sin(sinArg1);
	
	FVector location = owner->GetActorLocation();
	location.Z = waterSurface.Z + bodyZOffset + bodyZSwing;

	FRotator rotation = owner->GetActorRotation();
	rotation.Roll = rollTilt;
	owner->SetActorLocationAndRotation(location, rotation);
}
