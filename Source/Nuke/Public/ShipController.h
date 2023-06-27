// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShipController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AShipController : public AAIController
{
	GENERATED_BODY()
	
public:
	AShipController();

protected:

	virtual void BeginPlay() override;
};
