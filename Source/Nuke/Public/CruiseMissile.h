// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Missile.h"
#include "AircraftMovementComponent.h"
#include "CruiseMissile.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API ACruiseMissile : public AMissile
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere)
	UAircraftMovementComponent* movementComponent;

public:
	ACruiseMissile();

	virtual void BeginPlay() override;

	void SetCurrentSpeed(float speed);
};
