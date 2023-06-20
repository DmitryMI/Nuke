// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RadarComponent.h"
#include "Components/SphereComponent.h"
#include "RadarConeComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUKE_API URadarConeComponent : public URadarComponent
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly)
	float coneAngleDeg = 30.0f;

	UPROPERTY(EditAnywhere)
	USphereComponent* sphereCollision;

protected:
#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual bool IsActorTrackedByRadar(AActor* actor) const override;

	virtual void BeginPlay() override;
public:
	virtual void SetRadarRange(float radarRadius) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetConeAngleDeg(float angleDeg);
};
