// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnitOrder.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UUnitOrder : public UObject
{
	GENERATED_BODY()
	
};

UCLASS()
class NUKE_API UGenericPointOrder : public UUnitOrder
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	FVector targetLocation;

public:
	UFUNCTION(BlueprintCallable)
	FVector GetTargetLocation();
};

UCLASS()
class NUKE_API UGenericActorOrder : public UUnitOrder
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	AActor* targetActor;

public:
	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor();
};