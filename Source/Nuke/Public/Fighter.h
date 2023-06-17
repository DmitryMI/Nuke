// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft.h"
#include "AntiAirMissile.h"
#include "Fighter.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AFighter : public AAircraft
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAntiAirMissile> antiAirMissileType;

	UPROPERTY(EditDefaultsOnly)
	int antiAirMissileNumberMax = 6;
	
	UPROPERTY(EditAnywhere)
	int antiAirMissileNumber = 6;

	UPROPERTY(EditDefaultsOnly)
	float radarRange = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float radarConeAngle = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	float radarOmnidirectionalRange = 1000.0f;
};
