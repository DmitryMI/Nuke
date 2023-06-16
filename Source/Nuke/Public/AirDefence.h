#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GenericTeamAgentInterface.h"
#include "Components/SphereComponent.h"
#include "SurfaceToAirMissile.h"
#include "Attackable.h"
#include "AirDefence.generated.h"

UCLASS()
class NUKE_API AAirDefence : public APawn, public IGenericTeamAgentInterface, public IAttackable
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASurfaceToAirMissile> ammoType;

	UPROPERTY(EditDefaultsOnly)
	float weaponRange = 20000;

	UPROPERTY(EditDefaultsOnly)
	float weaponMaxSpeed = 3000;
	UPROPERTY(EditDefaultsOnly)
	float weaponAcceleration = 1500;

	UPROPERTY(EditDefaultsOnly)
	float weaponCooldown = 5.0f;

	UPROPERTY(VisibleAnywhere)
	bool bIsWeaponReady = true;

	UPROPERTY(EditAnywhere, Category = "Team")
	FGenericTeamId teamId = FGenericTeamId::NoTeam;

	UPROPERTY(EditAnywhere)
	USphereComponent* radarCollider;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> threatsInRadarRange;

	UPROPERTY()
	FTimerHandle cooldownHandle;

	UPROPERTY()
	FTimerHandle shootDelayHandle;

	UPROPERTY()
	AActor* lockedTarget = nullptr;

	UPROPERTY()
	bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere)
	TArray<ASurfaceToAirMissile*> managedMissiles;

	UFUNCTION()
	void OnCooldownExpired();

	UFUNCTION()
	void OnShootDelayExpired();

public:
	// Sets default values for this pawn's properties
	AAirDefence();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	AActor* Shoot(AActor* target);

	UFUNCTION(BlueprintCallable)
	void ShootDelayed(AActor* target, float delay);

	UFUNCTION()
	void OnRadarOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnRadarOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnManagedMissileDestroyed(ASurfaceToAirMissile* missile);

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetGenericTeamId(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const;

	UFUNCTION(BlueprintCallable)
	bool IsWeaponReady() const;

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetTrackedEnemies() const;

	UFUNCTION(BlueprintCallable)
	float GetWeaponMaxSpeed() const;

	UFUNCTION(BlueprintCallable)
	float GetWeaponAcceleration() const;

	UFUNCTION(BlueprintCallable)
	const TArray<ASurfaceToAirMissile*> GetManagedMissiles() const;

	virtual bool IsAlive() const override;

	virtual void ReceiveDamage(float damageAmount) override;

};
