// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RadarComponent.h"
#include "Components/SphereComponent.h"
#include "RadarSphereComponent.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API URadarSphereComponent : public URadarComponent
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere)
	float radius = 10000.0f;

	UPROPERTY(EditAnywhere)
	USphereComponent* sphereCollision;

protected:
#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	URadarSphereComponent();

	virtual void BeginPlay() override;

	virtual void SetRadarRadius(float radarRadius);

	float GetRadarRadius() const;
};
