// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarConeComponent.h"
#include "Attackable.h"

#if WITH_EDITOR  
void URadarConeComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "radarRange" && sphereCollision != nullptr)
	{
		sphereCollision->SetSphereRadius(GetRadarCollisionRadius());
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif


void URadarConeComponent::BeginPlay()
{
	sphereCollision = Cast<USphereComponent>(GetChildComponent(0));
	if (ensure(sphereCollision))
	{
		sphereCollision->SetCollisionProfileName(TEXT("RadarQuery"));
		sphereCollision->SetSphereRadius(GetRadarCollisionRadius());
	}

	Super::BeginPlay();
}


void URadarConeComponent::UpdateVisibilityOfActorsInRange()
{
	for (AActor* actorInRange : GetActorsInRadarRange())
	{
		IAttackable* attackable = Cast<IAttackable>(actorInRange);

		if (((int)attackable->GetMobilityEnvironmentType() & GetVisibleMobilityFlags()) == 0)
		{
			continue;
		}

		UFogOfWarComponent* fow = attackable->GetFogOfWarComponent();
		if (!fow)
		{
			continue;
		}

		fow->WitnessIfHasLineOfSight(GetOwner(), GetVisibilityRange());
	}
}

bool URadarConeComponent::IsInsideConeAngle(const FVector& location) const
{
	FVector direction = location - (GetOwner()->GetActorLocation() + GetRelativeLocation());

	if (direction.IsNearlyZero())
	{
		return true;
	}

	/*
	FRotator rotation = GetOwner()->GetActorRotation();
	FRotator angleToActor = direction.Rotation();
	

	// Incorrect, but fast
	double yawDeltaAngleDeg = FMath::FindDeltaAngleDegrees(rotation.Yaw, angleToActor.Yaw);
	double pitchDeltaAngleDeg = FMath::FindDeltaAngleDegrees(rotation.Pitch, angleToActor.Pitch);
	return FMath::Abs(yawDeltaAngleDeg) <= coneAngleDeg && FMath::Abs(pitchDeltaAngleDeg) <= coneAngleDeg;
	*/
	direction.Normalize();
	FVector forward = GetForwardVector();
	double dot = direction.Dot(forward);
	double angle = FMath::Acos(dot);
	double angleDeg = FMath::RadiansToDegrees(angle);
	return FMath::Abs(angleDeg) <= coneAngleDeg;
}

bool URadarConeComponent::TryTrackAndNotify(AActor* actor) const
{
	if (!actor || actor->IsPendingKill())
	{
		return false;
	}
	if (!IsInsideConeAngle(actor->GetActorLocation()))
	{
		return false;
	}

	return Super::TryTrackAndNotify(actor);
}

void URadarConeComponent::SetTrackingRange(float radarRadius)
{
	Super::SetTrackingRange(radarRadius);
	sphereCollision->SetSphereRadius(GetRadarCollisionRadius());
}

void URadarConeComponent::SetConeAngleDeg(float angleDeg)
{
	coneAngleDeg = angleDeg;
}
