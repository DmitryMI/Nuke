#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GenericTeamAgentInterface.h"
#include "Components/SphereComponent.h"
#include "SurfaceToAirMissile.h"
#include "AirDefence.generated.h"

UCLASS()
class NUKE_API AAirDefence : public APawn, public IGenericTeamAgentInterface
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
	TArray<AActor*> trackedEnemies;

	UPROPERTY()
	FTimerHandle cooldownHandle;

	UPROPERTY()
	FTimerHandle shootDelayHandle;

	UPROPERTY()
	AActor* lockedTarget = nullptr;

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

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Called every frame
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

};
