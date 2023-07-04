// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlaytimePlayerState.h"
#include "PlaytimeUtils.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UPlaytimeUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PlaytimeUtils")
	static APlaytimePlayerState* GetOwningPlayerState(AActor* actor);
};
