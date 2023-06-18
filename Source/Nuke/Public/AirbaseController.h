// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AirbaseController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AAirbaseController : public AAIController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ScoutLocation(const FVector& location);

	virtual void SetGenericTeamId(const FGenericTeamId& team) override;

	virtual  FGenericTeamId GetGenericTeamId() const override;
};
