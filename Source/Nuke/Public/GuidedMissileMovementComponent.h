// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GuidedMissileMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UGuidedMissileMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float acceleration = 1500;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float drag = 0.05;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float maxSpeed = 3000;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float maxTurnRateDegrees = 360;

	UPROPERTY(VisibleAnywhere, Category = "Movement|Runtime")
	bool bIsMovementFrozen = false;

	UPROPERTY(VisibleAnywhere, Category="Runtime")
	AActor* targetActor;

	UPROPERTY(VisibleAnywhere, Category = "Runtime")
	FVector targetLocation;

	UPROPERTY(VisibleAnywhere, Category = "Runtime")
	bool bUseTargetActorLocation = false;

	UPROPERTY(VisibleAnywhere, Category = "Runtime")
	bool bIsEngineOn = true;

	UPROPERTY(VisibleAnywhere, Category = "Runtime")
	float currentSpeed;

	

protected:
	virtual void RotateTowardsTarget(float DeltaTime);

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetIsEngineOn(bool isOn);

	AActor* GetTargetActor() const;
	void SetTargetActor(AActor* actor);
	void RequestMovementTowardsTargetActor();
	void RequestMovementTowardsLocation(const FVector& location);

	void FreezeMovement();

	float GetMaxSpeed() const;

	float GetAcceleration() const;
};
