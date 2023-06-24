// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimePlayerState.h"
#include "Net/UnrealNetwork.h"

void APlaytimePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlaytimePlayerState, teamId);
	DOREPLIFETIME(APlaytimePlayerState, playerUnits);
}

TArray<AActor*>& APlaytimePlayerState::GetPlayerUnitsMutable()
{
	return playerUnits;
}

const TArray<AActor*>& APlaytimePlayerState::GetPlayerUnits()
{
	return playerUnits;
}

FGenericTeamId APlaytimePlayerState::GetGenericTeamId() const
{
	return teamId;
}

void APlaytimePlayerState::SetGenericTeamId(const FGenericTeamId& team)
{
	teamId = team;
}
