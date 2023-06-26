// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Attackable.h"
#include "GenericTeamAgentInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Explosion.h"
#include "FogOfWarComponent.h"
#include "Missile.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMissileDestroyedEvent, AMissile*);

UCLASS()
class NUKE_API AMissile : public APawn, public IAttackable, public IGenericTeamAgentInterface
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
	float bodyRadius = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float bodyLength = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float maxLifetime = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AExplosion> explosionType;

	UPROPERTY(EditAnywhere, Category = "Physics")
	UCapsuleComponent* bodyCollider;

	UPROPERTY(VisibleAnywhere)
	UFogOfWarComponent* fogOfWarComponent;

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

	FMissileDestroyedEvent missileDestroyedEvent;

	UFUNCTION()
	void OnDestructionDelayExpired();

	UFUNCTION()
	void OnLifetimeExpired();

public:
	// Sets default values for this pawn's properties
	AMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnBodyOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	FMissileDestroyedEvent& OnMissileDestroyed();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void DestroyDelayed();

	UFUNCTION(BlueprintCallable)
	void Detonate();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const override;

	UFUNCTION(BlueprintCallable)
	void SetGenericTeamId(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	FGenericTeamId GetGenericTeamId() const;

	UFUNCTION(BlueprintCallable)
	float GetProximityFuseTriggerRange() const;

	UFUNCTION(BlueprintCallable)
	virtual EMobilityEnvironmentType GetMobilityEnvironmentType() const override;
};
