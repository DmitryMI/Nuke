// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GenericTeamAgentInterface.h"
#include "PlaytimePlayerState.h"
#include "PlaytimePlayerController.h"
#include "MatchProgressState.h"
#include "PlaytimeGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FMatchProgressStateChanged, APlaytimeGameState*, EMatchProgressState)

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimeGameState : public AGameState
{
	GENERATED_BODY()

private:

	static APlaytimeGameState* playtimeGameStateInstance;

	UPROPERTY(VisibleAnywhere, Replicated, ReplicatedUsing = OnRep_MatchProgressState)
	EMatchProgressState matchProgressState = EMatchProgressState::MPS_WaitingForStart;

	FMatchProgressStateChanged onMatchProgressStateChanged;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnRep_MatchProgressState(EMatchProgressState stateNew);
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

	UFUNCTION(BlueprintCallable)
	EMatchProgressState GetMatchProgressState() const;

	FMatchProgressStateChanged& OnMatchProgressStateChanged();

	void SetMatchProgressState(EMatchProgressState state);
};
