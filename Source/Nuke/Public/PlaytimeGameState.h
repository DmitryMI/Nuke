// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GenericTeamAgentInterface.h"
#include "PlaytimePlayerState.h"
#include "PlaytimePlayerController.h"
#include "PlaytimeGameState.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimeGameState : public AGameState
{
	GENERATED_BODY()

private:

	static APlaytimeGameState* playtimeGameStateInstance;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
public:
	virtual void BeginPlay() override;

	ETeamAttitude::Type SolveTeamAttitude(FGenericTeamId team1, FGenericTeamId team2) const;
	
	static ETeamAttitude::Type SolveTeamAttitudeStatic(FGenericTeamId team1, FGenericTeamId team2);

	UFUNCTION(BlueprintCallable)
	APlaytimePlayerController* GetPlayerControllerByTeam(const FGenericTeamId& teamId) const;

	UFUNCTION(BlueprintCallable)
	APlaytimePlayerState* GetPlayerStateByTeam(const FGenericTeamId& teamId) const;

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
};
