// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AntiAirMissileController.generated.h"

class UGuidedMissileMovementComponent;

/**
 * 
 */
UCLASS()
class NUKE_API AAntiAirMissileController : public AAIController
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	AActor* target;

	UPROPERTY(VisibleAnywhere)
	FVector rendezvousLocation;

	UGuidedMissileMovementComponent* GetGuidedMovementComponent() const;

protected:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void FlyTowardsLocation(const FVector& location);

	UFUNCTION(BlueprintCallable)
	void FlyTowardsTargetActor();

	UFUNCTION(BlueprintCallable)
	void FlyTowardsLinearInterceptionLocation();

public:
	UFUNCTION(BlueprintCallable)
	AActor* GetTarget() const;
	void SetTarget(AActor* targetNew);

	UFUNCTION(BlueprintCallable)
	void SetRendezvousLocation(const FVector& location);
};
