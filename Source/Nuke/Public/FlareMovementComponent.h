// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "FlareMovementComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUKE_API UFlareMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	float gravity = -10.0f;

	UPROPERTY(EditDefaultsOnly)
	float maxSpeed = 5000.0f;

	UPROPERTY(EditDefaultsOnly)
	float drag = 2;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
