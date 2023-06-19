// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/MovementComponent.h"
#include "BallisticMovementComponent.h"
#include "Attackable.h"
#include "BallisticMissile.generated.h"

UCLASS()
class NUKE_API ABallisticMissile : public APawn, public IGenericTeamAgentInterface, public IAttackable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float bodyRadius = 0.43;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float bodyLength = 5.3;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float targetTriggerDistance = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float maxLifetime = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float maxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float health = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Physics")
		UCapsuleComponent* bodyCollider;

	UPROPERTY(VisibleAnywhere, Category = "Team")
		FGenericTeamId teamId = FGenericTeamId::NoTeam;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlive = true;

	UPROPERTY(EditDefaultsOnly)
	float destructionDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float invulnerabilityDuration = 2.0f;

	UPROPERTY(VisibleAnywhere)
	FVector targetLocation;

	UPROPERTY()
		FTimerHandle destructionTimerHandle;

	UPROPERTY()
		FTimerHandle lifetimeTimerHandle;
	UPROPERTY()
		FTimerHandle invulnerabilityTimerHandle;

	UPROPERTY(EditAnywhere)
		UBallisticMovementComponent* ballisticMovement;

	UFUNCTION()
	void OnDestructionDelayExpired();

	UFUNCTION()
	void OnLifetimeExpired();

	UFUNCTION()
	void OnInvulnerabilityExpired();

public:
	// Sets default values for this pawn's properties
	ABallisticMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void DestroyDelayed();

	virtual void Detonate();

	UFUNCTION()
	void OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetGenericTeamId(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const;

	UFUNCTION(BlueprintCallable)
	void SetTargetAndLaunchLocation(const FVector& target, const FVector& start);

	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const override;

	UFUNCTION(BlueprintCallable)
	bool PredictLocation(float time, FVector& outLocation) const;

	UFUNCTION(BlueprintCallable)
	UBallisticMovementComponent* GetBallisticMovement() const;

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(float damage) override;
};
