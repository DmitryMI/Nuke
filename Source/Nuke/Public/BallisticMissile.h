// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/MovementComponent.h"
#include "BallisticMovementComponent.h"
#include "Missile.h"
#include "BallisticMissile.generated.h"

UCLASS()
class NUKE_API ABallisticMissile : public AMissile
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere)
	UBallisticMovementComponent* ballisticMovement;

	UPROPERTY(EditAnywhere)
	FVector targetLocation;

public:
	// Sets default values for this pawn's properties
	ABallisticMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetTargetAndLaunchLocation(const FVector& target, const FVector& start);

	UFUNCTION(BlueprintCallable)
	bool PredictLocation(float time, FVector& outLocation) const;

	UFUNCTION(BlueprintCallable)
	UBallisticMovementComponent* GetBallisticMovement() const;

	virtual void DestroyDelayed() override;

	virtual float GetStealthFactor() const override;
};
