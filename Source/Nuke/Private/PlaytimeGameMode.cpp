// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimeGameMode.h"
#include "PlaytimeGameState.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"


void APlaytimeGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	AddBot();
}

void APlaytimeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

bool APlaytimeGameMode::ReadyToStartMatch_Implementation()
{
	// If bDelayed Start is set, wait for a manual match start
	if (bDelayedStart)
	{
		return false;
	}

	// By default start when we have > 0 players
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		return NumPlayers + NumBots == MaxNumPlayers;
	}
	return false;
}

bool APlaytimeGameMode::ReadyToEndMatch_Implementation()
{
	return false;
}

void APlaytimeGameMode::AddBot()
{
	if (!BotControllerType)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to add Bot: BotControllerType is nullptr!"));
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save player controllers into a map
	ANukeBotController* botController = GetWorld()->SpawnActor<ANukeBotController>(BotControllerType, SpawnInfo);

	RestartPlayer(botController);
	botControllers.Add(botController);
	NumBots++;
}
