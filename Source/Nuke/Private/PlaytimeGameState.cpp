// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimeGameState.h"
#include "Net/UnrealNetwork.h"

APlaytimeGameState* APlaytimeGameState::playtimeGameStateInstance = nullptr;

void APlaytimeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlaytimeGameState, matchProgressState);
}

void APlaytimeGameState::OnRep_MatchProgressState(EMatchProgressState stateNew)
{
	onMatchProgressStateChanged.Broadcast(this, stateNew);
}

void APlaytimeGameState::BeginPlay()
{
	Super::BeginPlay();

	playtimeGameStateInstance = this;

	FGenericTeamId::SetAttitudeSolver(SolveTeamAttitudeStatic);
}

ETeamAttitude::Type APlaytimeGameState::SolveTeamAttitude(FGenericTeamId team1, FGenericTeamId team2) const
{
	if (team1 == FGenericTeamId::NoTeam || team2 == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}

	if (team1 == team2)
	{
		return ETeamAttitude::Friendly;
	}

	return ETeamAttitude::Hostile;
}

ETeamAttitude::Type APlaytimeGameState::SolveTeamAttitudeStatic(FGenericTeamId team1, FGenericTeamId team2)
{
	check(playtimeGameStateInstance);
	return playtimeGameStateInstance->SolveTeamAttitude(team1, team2);
}

APlaytimePlayerController* APlaytimeGameState::GetPlayerControllerByTeam(const FGenericTeamId& teamId) const
{
	APlaytimePlayerState* playerState = GetPlayerStateByTeam(teamId);
	if (!playerState)
	{
		return nullptr;
	}

	return Cast<APlaytimePlayerController>(playerState->GetPlayerController());
}

APlaytimePlayerState* APlaytimeGameState::GetPlayerStateByTeam(const FGenericTeamId& teamId) const
{
	if (teamId == FGenericTeamId::NoTeam)
	{
		return nullptr;
	}

	check(PlayerArray.Num() > teamId.GetId());
	APlayerState* playerState = PlayerArray[teamId.GetId()];
	return Cast<APlaytimePlayerState>(playerState);
}

void APlaytimeGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	int index = PlayerArray.IndexOfByKey(PlayerState);
	APlaytimePlayerState* playtimePlayerState = Cast<APlaytimePlayerState>(PlayerState);
	playtimePlayerState->SetGenericTeamId(index);
}

void APlaytimeGameState::RemovePlayerState(APlayerState* PlayerState)
{
	APlaytimePlayerState* playtimePlayerState = Cast<APlaytimePlayerState>(PlayerState);
	playtimePlayerState->SetGenericTeamId(FGenericTeamId::NoTeam);
	Super::RemovePlayerState(PlayerState);
}

EMatchProgressState APlaytimeGameState::GetMatchProgressState() const
{
	return matchProgressState;
}

FMatchProgressStateChanged& APlaytimeGameState::OnMatchProgressStateChanged()
{
	return onMatchProgressStateChanged;
}

void APlaytimeGameState::SetMatchProgressState(EMatchProgressState state)
{
	if (GEngine->GetNetMode(GetWorld()) == ENetMode::NM_Client)
	{
		return;
	}

	matchProgressState = state;

	// OnRep not called on servers, doing it manually
	OnRep_MatchProgressState(state);
}
