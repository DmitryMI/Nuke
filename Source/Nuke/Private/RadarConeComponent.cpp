// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarConeComponent.h"

#if WITH_EDITOR  
void URadarConeComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "range" && sphereCollision != nullptr)
	{
		sphereCollision->SetSphereRadius(range);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
bool URadarConeComponent::IsActorTrackedByRadar(AActor* actor) const
{
	bool isInsideCone = false;

	FVector direction = actor->GetActorLocation() - (GetOwner()->GetActorLocation() + GetRelativeLocation());
	if (direction.IsNearlyZero())
	{
		isInsideCone = true;
	}
	else
	{
		FRotator rotation = GetComponentRotation();
		FRotator angleToActor = direction.Rotation();

		// Incorrect, but fast
		double yawDeltaAngleDeg = FMath::FindDeltaAngleDegrees(rotation.Yaw, angleToActor.Yaw);
		double pitchDeltaAngleDeg = FMath::FindDeltaAngleDegrees(rotation.Pitch, angleToActor.Pitch);
		isInsideCone = FMath::Abs(yawDeltaAngleDeg) <= coneAngleDeg && FMath::Abs(pitchDeltaAngleDeg) < coneAngleDeg;
	}

	if (!isInsideCone)
	{
		return false;
	}

	return Super::EvaluateTrackingConditions(actor);
}
void URadarConeComponent::BeginPlay()
{
	sphereCollision = Cast<USphereComponent>(GetChildComponent(0));
	if (ensure(sphereCollision))
	{
		sphereCollision->SetCollisionProfileName(TEXT("RadarQuery"));
		sphereCollision->SetSphereRadius(range);
	}

	Super::BeginPlay();

	float coneAngleRad = FMath::DegreesToRadians(coneAngleDeg);
	coneAngleCos = FMath::Cos(coneAngleRad);
}
#endif


void URadarConeComponent::SetRadarRange(float radarRadius)
{
	range = radarRadius;
	sphereCollision->SetSphereRadius(range);
}

void URadarConeComponent::SetConeAngleDeg(float angleDeg)
{
	coneAngleDeg = angleDeg;
	float coneAngleRad = FMath::DegreesToRadians(coneAngleDeg);
	coneAngleCos = FMath::Cos(coneAngleRad);
}
