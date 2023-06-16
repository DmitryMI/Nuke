// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GenericTeamAgentInterface.h"
#include "GuidedMissileMovementComponent.h"
#include "Explosion.h"
#include "Attackable.h"
#include "SurfaceToAirMissile.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMissileDestroyedEvent, ASurfaceToAirMissile*);

UCLASS()
class NUKE_API ASurfaceToAirMissile : public APawn, public IGenericTeamAgentInterface, public IAttackable
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float explosionRadius = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float explosionDamageMax = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float proximityFuseTriggerRange = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float proximityFuseDelay = 0.1;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float bodyRadius = 0.43;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float bodyLength = 5.3;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float maxLifetime = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USphereComponent* proximityCollider;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AExplosion> explosionType;

	UPROPERTY(EditAnywhere, Category = "Physics")
	UCapsuleComponent* bodyCollider;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	FGenericTeamId teamId = FGenericTeamId::NoTeam;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlive = true;

	UPROPERTY(EditDefaultsOnly)
	float destructionDelay = 5.0f;

	UPROPERTY()
	FTimerHandle destructionTimerHandle;

	UPROPERTY()
	FTimerHandle lifetimeTimerHandle;

	UPROPERTY(EditAnywhere)
	UGuidedMissileMovementComponent* guidedMovement;

	UFUNCTION()
	void OnDestructionDelayExpired();

	UFUNCTION()
	void OnLifetimeExpired();

public:
	// Sets default values for this pawn's properties
	ASurfaceToAirMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnProximityFuseCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	FMissileDestroyedEvent missileDestroyedEvent;

public:
	FMissileDestroyedEvent& OnMissileDestroyed();

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetLockedOnTarget(AActor* actor);

	UFUNCTION(BlueprintCallable)
	AActor* GetLockedOnTarget() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetGenericTeamId(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const;

	UFUNCTION(BlueprintCallable)
	void DestroyDelayed();

	UFUNCTION(BlueprintCallable)
	void Detonate();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const override;

	UFUNCTION(BlueprintCallable)
	void SetRendezvousLocation(const FVector& location);

	UFUNCTION(BlueprintCallable)
	float GetMaxSpeed() const;

	UFUNCTION(BlueprintCallable)
	float GetAcceleration() const;

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(float damageAmount);
};
