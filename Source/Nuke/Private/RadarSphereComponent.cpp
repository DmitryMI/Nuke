// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarSphereComponent.h"

#if WITH_EDITOR  
void URadarSphereComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "radius" && sphereCollision != nullptr)
	{
		sphereCollision->SetSphereRadius(radius);
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
		sphereCollision->SetSphereRadius(radius);
	}

	Super::BeginPlay();
}

void URadarSphereComponent::SetRadarRadius(float radarRadius)
{
	radius = radarRadius;

	if (sphereCollision)
	{
		sphereCollision->SetSphereRadius(radarRadius);
	}
}

float URadarSphereComponent::GetRadarRadius() const
{
	return radius;
}
