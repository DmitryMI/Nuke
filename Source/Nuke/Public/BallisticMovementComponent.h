// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "BallisticMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UBallisticMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	FVector targetLocation;

	UPROPERTY(VisibleAnywhere)
	FVector launchLocation;

	UPROPERTY(VisibleAnywhere)
	float arcRadius;

	UPROPERTY(VisibleAnywhere)
	float arcLength;

	UPROPERTY(VisibleAnywhere)
	float totalDistance;

	UPROPERTY(EditDefaultsOnly)
	float acceleration = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	float maxSpeed = 5000.0;

	UPROPERTY(VisibleAnywhere)
	float currentSpeed;

	UPROPERTY(VisibleAnywhere)
	float distanceTraveled;

	UPROPERTY(VisibleAnywhere)
	bool bMovementFrozen = false;
	
protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

public:
	void SetTargetAndLaunchLocation(const FVector& target, const FVector& launch);

	void FreezeMovement();

	UFUNCTION(BlueprintCallable)
	bool PredictLocation(float time, FVector& outLocation) const;

	UFUNCTION(BlueprintCallable)
	FVector GetTargetLocation() const;
};
