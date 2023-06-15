// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BallisticMissile.h"
#include "GenericTeamAgentInterface.h"
#include "Silo.generated.h"

UCLASS()
class NUKE_API ASilo : public APawn, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABallisticMissile> ammoType;

	UPROPERTY(EditDefaultsOnly)
	float weaponCooldown = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float errorRadius = 500.0f;

	UPROPERTY(VisibleAnywhere)
	bool bIsWeaponReady = true;

	UPROPERTY(EditAnywhere, Category = "Team")
	FGenericTeamId teamId = FGenericTeamId::NoTeam;
	
	UPROPERTY()
	FTimerHandle cooldownHandle;

	UFUNCTION()
	void OnCooldownExpired();

public:
	// Sets default values for this pawn's properties
	ASilo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	AActor* Shoot(const FVector& targetLocation);

	UFUNCTION(BlueprintCallable)
		virtual void SetGenericTeamId(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
		virtual FGenericTeamId GetGenericTeamId() const;
};
