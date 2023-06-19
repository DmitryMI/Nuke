// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Fighter.h"
#include "Attackable.h"
#include "GenericTeamAgentInterface.h"
#include "Airbase.generated.h"

UCLASS()
class NUKE_API AAirbase : public APawn, public IAttackable, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFighter> fighterType;

	UPROPERTY(EditAnywhere)
	int fightersNumber = 5;

	UPROPERTY(EditDefaultsOnly)
	float launchCooldownTime = 3.0f;

	UPROPERTY(EditAnywhere)
	FGenericTeamId teamId;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlive = true;

	UPROPERTY()
	bool bIsLaunchReady = true;

	FTimerHandle launchCooldownHandle;

	UFUNCTION()
	void OnLauchStateCooldownExpired();
public:
	// Sets default values for this pawn's properties
	AAirbase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AAircraft* LaunchAircraft(TSubclassOf<AAircraft> aircraftType);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsReadyToLaunch() const;

	UFUNCTION(BlueprintCallable)
	AFighter* LaunchFigher();

	UFUNCTION(BlueprintCallable)
	int GetNumberOfFighters() const;

	UFUNCTION(BlueprintCallable)
	void SetNumberOfFighters(int number);

	UFUNCTION(BlueprintCallable)
	FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void SetGenericTeamId(const FGenericTeamId& team) override;

	virtual bool IsAlive() const override;

};
