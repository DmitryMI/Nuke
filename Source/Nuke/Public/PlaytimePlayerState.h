// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "City.h"
#include "PlaytimePlayerState.generated.h"


USTRUCT(BlueprintType)
struct FUndeployedUnitCounters
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int Airbases = 4;
	UPROPERTY(EditAnywhere)
	int Silos = 4;
	UPROPERTY(EditAnywhere)
	int AirDefences = 8;
	UPROPERTY(EditAnywhere)
	int Cruisers = 8;
	UPROPERTY(EditAnywhere)
	int AircraftCarriers = 2;
	UPROPERTY(EditAnywhere)
	int Submarines = 4;
};

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimePlayerState : public APlayerState, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Replicated)
	FUndeployedUnitCounters undeployedUnitCounters;

	UPROPERTY(VisibleAnywhere, Replicated)
	FGenericTeamId teamId;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<AActor*> playerUnits;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<ACity*> playerCities;

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

public:
	UFUNCTION(BlueprintCallable)
	TArray<AActor*>& GetPlayerUnitsMutable();

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetPlayerUnits() const;

	UFUNCTION(BlueprintCallable)
	TArray<ACity*>& GetPlayerCitiesMutable();

	UFUNCTION(BlueprintCallable)
	const TArray<ACity*>& GetPlayerCities() const;

	UFUNCTION(BlueprintCallable)
	FUndeployedUnitCounters& GetUndeployedUnitCountersMutable();

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void SetGenericTeamId(const FGenericTeamId& team) override;
};
