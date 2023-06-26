// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GenericTeamAgentInterface.h"
#include "PlaytimeGameState.h"
#include "PlaytimePlayerState.h"
#include "FogOfWarComponent.generated.h"

USTRUCT(BlueprintType)
struct FFogOfWarInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bIsVisible = false;
	UPROPERTY(EditAnywhere)
	double WitnessingTimestamp = -1000.0f;

	UPROPERTY(EditAnywhere)
	uint64 WitnessingFrameNumber = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NUKE_API UFogOfWarComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere)
	float visibilityInfosUpdatePeriod = 0.5f;

	UPROPERTY(EditAnywhere)
	float visibilityLingerTime = 2.0f;

	UPROPERTY(VisibleAnywhere)
	TArray<FFogOfWarInfo> visibilityArray;

	UPROPERTY()
	FTimerHandle visibilityUpdateHandle;

	UFUNCTION()
	void UpdateVisibilityInfos();

	UFUNCTION()
	FGenericTeamId GetOwnerTeam() const;

	UPROPERTY(Transient)
	APlaytimePlayerState* localPlayerState;

public:	
	// Sets default values for this component's properties
	UFogOfWarComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool IsVisibleForTeam(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	void WitnessUnconditional(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	void WitnessIfHasLineOfSight(AActor* sightSourceActor, float maxDistance, float timeTolerance);
};
