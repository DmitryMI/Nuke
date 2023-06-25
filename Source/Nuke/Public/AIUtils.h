// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "BallisticMovementComponent.h"
#include "AircraftWaypoint.h"
#include "City.h"
#include "Missile.h"
#include "AIUtils.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UAIUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool AreEnemies(AActor* actor1, AActor* actor2);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool CalculateInterceptionLinear(
		const FVector& targetLocation,
		const FVector& targetVelocity,
		const FVector& launchLocation,
		double projectileSpeed,
		FVector& outInterceptionLocation);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool CalculateInterceptionLinearTime(const FVector& targetRelativePosition, const FVector& targetVelocity, float shotSpeed, float& outTime);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static int QuadraticEquation(double a, double b, double c, double& x1, double& x2);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool CalculateInterceptionBallistic(UBallisticMovementComponent* targetMovement, const FVector& launchLocation,
		double projectileInitialSpeed, double projectileAcceleration, double projectileMaxSpeed,
		FVector& outInterceptionLocation, double& outInterceptionTime, double& launchDelay);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool IsAttackableAlive(AActor* attackableActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static void AttackableReceiveDamage(AActor* attackableActor, float damageAmount);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static AActor* GetClosestActor(const FVector& fromLocation, TArray<AActor*> actors);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static TArray<FAircraftWaypoint> GenerateRandomAircraftPath(FVector startingLocation, float segmentLength = 5000.0f, int points = 10, FVector2D altitudeRange = FVector2D(1000.0f, 5000.0f));

	UFUNCTION(BlueprintCallable, Category = "AI")
	static TArray<AActor*> GetEnemyCities(AActor* teamAgentActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static AActor* GetRandomActor(const TArray<AActor*>& actors);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool IsActorTrackedByRadar(AActor* radarActor, AActor* trackableActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool GetTrackedThreatsByRadar(AActor* radarActor, TArray<AActor*>& outTrackedThreats);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool GetTrackingMissiles(AActor* targetActor, TArray<AMissile*>& outMissiles);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static int GetNumberOfUnitsTargetingActor(AActor* teamContextActor, AActor* targetActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool CheckVisibility(AActor* sightSource, AActor* sightTarget);
};
