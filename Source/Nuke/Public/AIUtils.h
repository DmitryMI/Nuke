// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "BallisticMovementComponent.h"
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
};
