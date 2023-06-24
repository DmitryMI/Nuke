// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "PlaytimeGameSession.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimeGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	virtual void InitOptions(const FString& Options);
};
