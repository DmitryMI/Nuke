// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ShipMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UShipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	float maxSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere)
	float currentSpeed = 0;

	UPROPERTY(EditDefaultsOnly)
	float acceleration = 20.0f;

	UPROPERTY(EditDefaultsOnly)
	float deceleration = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float maxAngularSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float rollTiltAngleMax = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float rollTiltSpeed = 1;

	UPROPERTY(EditAnywhere)
	float bodyZOffset = -30;

	UPROPERTY(EditAnywhere)
	float bodyZSwingMax = 10.0f;

	UPROPERTY(EditAnywhere)
	float bodyZSwingSpeed = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float turnMaxTilt = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float targetLocationTolerance = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float minimalDepth = 1000.0f;


protected:
	virtual void AnimateShipBuoyancy(float DeltaTime, float& zOffset, float& rollOffset);

	virtual void GetVelocityAndRotationFromInput(float DeltaTime, FVector& nextVelocity, FRotator& nextRotation);

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool FindWaterUnderShip(float& waterSurfaceZ, float& waterDepth) const;

	bool TestLocation(const FVector& location, float& waterSurfaceZ, float& waterDepth) const;

public:
	UShipMovementComponent();

	virtual void RequestPathMove(const FVector& MoveInput) override;
};
