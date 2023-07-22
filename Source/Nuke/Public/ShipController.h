// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ShipController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AShipController : public AUnitController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBehaviorTree* HoldPositionTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBehaviorTree* ScoutLocationTree;

public:
	AShipController();

protected:

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void SetGenericTeamId(const FGenericTeamId& team) override;

	UFUNCTION(BlueprintCallable)
	virtual bool IssueGenericPointOrder(const FVector& location, bool queue = false);

	UFUNCTION(BlueprintCallable)
	virtual bool IssueGenericActorOrder(AActor* actor, bool queue = false);

	UFUNCTION(BlueprintCallable)

	virtual bool IssueStopOrder() override;
};
