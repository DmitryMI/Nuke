// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "PlaytimePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimePlayerState : public APlayerState, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, Replicated)
	FGenericTeamId teamId;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<AActor*> playerUnits;

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

public:
	UFUNCTION(BlueprintCallable)
	TArray<AActor*>& GetPlayerUnitsMutable();

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetPlayerUnits();

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void SetGenericTeamId(const FGenericTeamId& team) override;
};
