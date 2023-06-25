// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CooperativeController.h"
#include "AirDefenceController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AAirDefenceController : public AAIController, public ICooperativeController
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable)
	bool GetEngagedTargets(TArray<AActor*>& outTargets) const;

	UFUNCTION(BlueprintCallable)
	AActor* SuggestBestTarget(TArray<AActor*> possibleTargets) const;

	UFUNCTION(BlueprintCallable)
	int GetNumOwnMissilesTargetingActor(AActor* actor) const;

	UFUNCTION(BlueprintCallable)
	int GetNumberOfUnitsTargetingActor(AActor* targetActor) const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsTargetEngaged(AActor* targetActor) const override;

	virtual void SetGenericTeamId(const FGenericTeamId& team);

	virtual FGenericTeamId GetGenericTeamId() const;
};
