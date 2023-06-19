// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft.h"
#include "AntiAirMissile.h"
#include "RadarConeComponent.h"
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
	float antiAirMissileCooldown= 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float radarRange = 20000.0f;

	UPROPERTY(EditDefaultsOnly)
	float radarConeAngle = 45.0f;

	UPROPERTY(EditAnywhere)
	URadarConeComponent* fighterRadar;

	UPROPERTY(VisibleAnywhere)
	bool bIsAntiAirMissileReady = true;

	UPROPERTY(VisibleAnywhere)
	TArray<AAntiAirMissile*> managedAntiAirMissiles;

	UPROPERTY()
	FTimerHandle antiAirMissileCooldownHandle;

	UFUNCTION()
	void OnAntiAirMissileCooldownExpired();

protected:
	virtual void BeginPlay() override;

	virtual void OnManagedMissileDestroyed(AMissile* sender);
public:
	AFighter();

	UFUNCTION(BlueprintCallable)
	AAntiAirMissile* LaunchAntiAirMissile(AActor* shootTarget);

	const TArray<AAntiAirMissile*>& GetManagedAntiAirMissiles() const;

	UFUNCTION(BlueprintCallable)
	bool IsAntiAirMissileReady() const;

	UFUNCTION(BlueprintCallable)
	bool HasAntiAirMissiles() const;
};
