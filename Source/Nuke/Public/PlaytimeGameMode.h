// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NukeBotController.h"
#include "PlaytimeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimeGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	int MaxNumPlayers = 2;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ANukeBotController> BotControllerType;

	TArray<ANukeBotController*> botControllers;
	
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual bool ReadyToStartMatch_Implementation() override;

	virtual bool ReadyToEndMatch_Implementation() override;

	void AddBot();
};
