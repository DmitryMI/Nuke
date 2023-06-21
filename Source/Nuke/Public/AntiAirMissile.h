// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Missile.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GuidedMissileMovementComponent.h"
#include "Explosion.h"
#include "Radar.h"
#include "AntiAirMissile.generated.h"


UCLASS()
class NUKE_API AAntiAirMissile : public AMissile, public IRadar
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UGuidedMissileMovementComponent* guidedMovement;

	UPROPERTY(EditDefaultsOnly)
	double radarRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly)
	double radarConeAngle = 80.0f;

	UPROPERTY(EditDefaultsOnly)
	double flareDistractionChancePerSecond = 0.02;

public:
	// Sets default values for this pawn's properties
	AAntiAirMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnProximityFuseCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetLockedOnTarget(AActor* actor);

	UFUNCTION(BlueprintCallable)
	AActor* GetLockedOnTarget() const;

	virtual void DestroyDelayed() override;

	UFUNCTION(BlueprintCallable)
	void SetRendezvousLocation(const FVector& location);

	UFUNCTION(BlueprintCallable)
	float GetMaxSpeed() const;

	UFUNCTION(BlueprintCallable)
	float GetAcceleration() const;

	virtual bool GetTrackedThreats(TArray<AActor*>& outThreats) const override;
	virtual bool IsActorTrackedByRadar(AActor* actor) const override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsTargetTrackedByMissileOrInstigator() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetCurrentSpeed(float speed);
};
