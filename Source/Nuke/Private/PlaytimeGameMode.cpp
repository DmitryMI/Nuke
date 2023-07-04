// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimeGameMode.h"
#include "PlaytimeGameState.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

void APlaytimeGameMode::FixMainNavData() const
{
	ARecastNavMesh* navMesh = Cast<ARecastNavMesh>(UGameplayStatics::GetActorOfClass(GetWorld(), ARecastNavMesh::StaticClass()));

	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!navSystem->MainNavData)
	{
		if (navMesh)
		{
			navSystem->MainNavData = navMesh;
			UE_LOG(LogTemp, Warning, TEXT("MainNavData was nullptr! Set to %s"), *navMesh->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MainNavData is nullptr and ARecastNavMesh actor was not found!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("MainNavData is %s"), *navSystem->MainNavData->GetName());
	}
}

void APlaytimeGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FixMainNavData();

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
