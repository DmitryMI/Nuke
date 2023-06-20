// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarConeComponent.h"

#if WITH_EDITOR  
void URadarConeComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "radarRange" && sphereCollision != nullptr)
	{
		sphereCollision->SetSphereRadius(GetRadarRange());
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
		sphereCollision->SetSphereRadius(GetRadarRange());
	}

	Super::BeginPlay();
}
#endif


void URadarConeComponent::SetRadarRange(float radarRadius)
{
	Super::SetRadarRange(radarRadius);
	sphereCollision->SetSphereRadius(radarRadius);
}

void URadarConeComponent::SetConeAngleDeg(float angleDeg)
{
	coneAngleDeg = angleDeg;
}
