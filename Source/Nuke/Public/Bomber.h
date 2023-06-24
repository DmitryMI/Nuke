// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft.h"
#include "CruiseMissile.h"
#include "Bomber.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API ABomber : public AAircraft
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACruiseMissile> cruiseMissileType;

	UPROPERTY(EditDefaultsOnly)
	int cruiseMissileNumberMax = 6;

	UPROPERTY(EditAnywhere)
	int cruiseMissileNumber = 6;

	UPROPERTY(EditDefaultsOnly)
	float cruiseMissileCooldown = 2.0f;

	UPROPERTY(VisibleAnywhere)
	bool bIsCruiseMissileReady = true;

	UPROPERTY()
	FTimerHandle cruiseMissileCooldownHandle;

	UFUNCTION()
	void OnCruiseMissileCooldownExpired();

public:
	UFUNCTION(BlueprintCallable)
	ACruiseMissile* LaunchCruiseMissile(const FVector& targetLocation);

	UFUNCTION(BlueprintCallable)
	bool IsCruiseMissileReady() const;

	UFUNCTION(BlueprintCallable)
	bool HasCruiseMissiles() const;
	
};
