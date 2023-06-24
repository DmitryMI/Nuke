// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Attackable.h"
#include "Components/CapsuleComponent.h"
#include "AircraftMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Radar.h"
#include "FlareDecoy.h"
#include "RadarDetectorComponent.h"
#include "Aircraft.generated.h"

class AAirbase;

UCLASS()
class NUKE_API AAircraft : public APawn, public IAttackable, public IGenericTeamAgentInterface, public IRadar
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere)
	float maxHealth = 100.0f;

	UPROPERTY(EditAnywhere)
	float health = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float stealthFactor = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float flaresCooldown = 0.25f;

	bool bFlaresReady = true;

	UPROPERTY(EditDefaultsOnly)
	int flaresCharges = 24;

	UPROPERTY(EditDefaultsOnly)
	float flaresDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFlareDecoy> flareType;

	UPROPERTY(VisibleAnywhere)
	TArray<AFlareDecoy*> flaresActive;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere)
	float fuel = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	float bodyLength = 20.0f;
	UPROPERTY(EditDefaultsOnly)
	float bodyRadius = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float landingDistance = 100.0f;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* bodyCollider;

	UPROPERTY(EditAnywhere)
	UAircraftMovementComponent* aircraftMovement;

	UPROPERTY(EditAnywhere)
	URadarDetectorComponent* radarDetector;

	UPROPERTY(EditAnywhere)
	FGenericTeamId teamId;

	UPROPERTY(EditDefaultsOnly)
	float destroyDelay = 5.0f;

	UPROPERTY()
	FTimerHandle destroyDelayedHandle;

	UPROPERTY()
	FTimerHandle flareCooldownHandle;

	void OnDestroyDelayedExpired();

	void OnFlareCooldownExpired();

public:
	// Sets default values for this pawn's properties
	AAircraft();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void DestroyDelayed();


#if WITH_EDITOR  
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool IsAlive() const override;

	virtual void ReceiveDamage(float damageAmount) override;

	void SetGenericTeamId(const FGenericTeamId& team) override;

	FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	UAircraftMovementComponent* GetAircraftMovementComponent() const;

	UFUNCTION(BlueprintCallable)
	virtual TArray<AActor*> GetTrackedThreatsArray() const;

	UFUNCTION(BlueprintCallable)
	virtual bool GetTrackedThreats(TArray<AActor*>& outThreats) const override;

	UFUNCTION(BlueprintCallable)
	bool CanLandOnBase(AAirbase* airbase) const;

	UFUNCTION(BlueprintCallable)
	void LandOnBase(AAirbase* airbase);

	UFUNCTION(BlueprintCallable)
	virtual bool IsActorTrackedByRadar(AActor* actor) const override;

	UFUNCTION(BlueprintCallable)
	virtual EMobilityEnvironmentType GetMobilityEnvironmentType() const override;

	UFUNCTION(BlueprintCallable)
	virtual AFlareDecoy* DeployFlareDecoy();

	UFUNCTION(BlueprintCallable)
	bool HasFlares() const;

	UFUNCTION(BlueprintCallable)
	bool AreFlaresReady() const;

	UFUNCTION(BlueprintCallable)
	const TArray<AFlareDecoy*>& GetActiveFlayers() const;

	UFUNCTION(BlueprintCallable)
	int GetActiveFlayersCount() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetStealthFactor() const override;
};
