#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GenericTeamAgentInterface.h"
#include "RadarSphereComponent.h"
#include "AntiAirMissile.h"
#include "Attackable.h"
#include "Radar.h"
#include "AirDefence.generated.h"

UCLASS()
class NUKE_API AAirDefence : public APawn, public IGenericTeamAgentInterface, public IAttackable, public IRadar
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAntiAirMissile> ammoType;

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
	URadarSphereComponent* radarComponent;

	UPROPERTY()
	FTimerHandle cooldownHandle;

	UPROPERTY()
	FTimerHandle shootDelayHandle;

	UPROPERTY()
	AActor* lockedTarget = nullptr;

	UPROPERTY()
	bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere)
	TArray<AAntiAirMissile*> managedMissiles;

	UPROPERTY(VisibleAnywhere)
	UFogOfWarComponent* fogOfWarComponent;

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
	void OnManagedMissileDestroyed(AMissile* missile);

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
	virtual bool GetTrackedThreats(TArray<AActor*>& outThreats) const;
	UFUNCTION(BlueprintCallable)
	virtual bool IsActorTrackedByRadar(AActor* actor) const;

	UFUNCTION(BlueprintCallable)
	float GetWeaponMaxSpeed() const;

	UFUNCTION(BlueprintCallable)
	float GetWeaponAcceleration() const;

	UFUNCTION(BlueprintCallable)
	const TArray<AAntiAirMissile*>& GetManagedMissiles() const;

	virtual bool IsAlive() const override;

	virtual void ReceiveDamage(float damageAmount) override;

	virtual UFogOfWarComponent* GetFogOfWarComponent() const override;
};
