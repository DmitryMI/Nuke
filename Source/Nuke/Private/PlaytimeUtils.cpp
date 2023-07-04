// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimeUtils.h"
#include "PlaytimeGameState.h"
#include "GenericTeamAgentInterface.h"

APlaytimePlayerState* UPlaytimeUtils::GetOwningPlayerState(AActor* actor)
{
	if (!actor)
	{
		return nullptr;
	}

	UWorld* world = actor->GetWorld();
	APlaytimeGameState* gameState = world->GetGameState<APlaytimeGameState>();
	check(gameState);

	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(actor);
	if (!teamAgent)
	{
		return nullptr;
	}

	FGenericTeamId team = teamAgent->GetGenericTeamId();
	return gameState->GetPlayerStateByTeam(team);
}