// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UnitControllerInterface.h"
#include "UnitController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AUnitController : public AAIController, public IUnitControllerInterface
{
	GENERATED_BODY()

public:
	virtual bool IssueGenericPointOrder(const FVector& location, bool queue = false) override;
	virtual bool IssueGenericActorOrder(AActor* actor, bool queue = false) override;
	virtual bool IssueStopOrder() override;

	UFUNCTION(BlueprintCallable)
	virtual void ReportOrderFinished(bool bSuccessful);
};
