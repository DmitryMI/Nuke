// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarSphereComponent.h"

#if WITH_EDITOR  
void URadarSphereComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "radarRange" && sphereCollision != nullptr)
	{
		sphereCollision->SetSphereRadius(GetRadarCollisionRadius());
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif


URadarSphereComponent::URadarSphereComponent()
{
	
}

void URadarSphereComponent::BeginPlay()
{
	sphereCollision = Cast<USphereComponent>(GetChildComponent(0));
	if (ensure(sphereCollision))
	{
		sphereCollision->SetCollisionProfileName(TEXT("RadarQuery"));
		sphereCollision->SetSphereRadius(GetRadarCollisionRadius());
	}

	Super::BeginPlay();
}

void URadarSphereComponent::SetTrackingRange(float radarRadius)
{
	Super::SetTrackingRange(radarRadius);

	if (sphereCollision)
	{
		sphereCollision->SetSphereRadius(radarRadius);
	}
}