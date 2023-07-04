// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UnitController.h"
#include "ShipController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AShipController : public AAIController, public IUnitController
{
	GENERATED_BODY()
	
public:
	AShipController();

protected:

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void SetGenericTeamId(const FGenericTeamId& team) override;
};
