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

	virtual void BeginPlay() override;

	virtual void UpdateVisibilityOfActorsInRange() override;

	bool IsInsideConeAngle(const FVector& location) const;

	virtual bool CanTrackActorInRadarRange(AActor* actor) const override;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	virtual void DrawDebugRadarShape() override;
#endif

public:
	virtual void SetTrackingRange(float radarRadius) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetConeAngleDeg(float angleDeg);
};
