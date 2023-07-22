// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AircraftWaypoint.h"
#include "AirPath.h"
#include "RadarDetectorComponent.h"
#include "PlaytimeGameState.h"
#include "PlaytimePlayerState.h"
#include "Fighter.h"
#include "CooperativeController.h"
#include "UnitControllerInterface.h"
#include "AirbaseInterface.h"
#include "AircraftController.generated.h"

class AAirbase;

/**
 * 
 */
UCLASS()
class NUKE_API AAircraftController : public AAIController, public ICooperativeController, public IUnitControllerInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	float pathToleranceDefault = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	float scanForTrackingMissilesInterval = 1.0f;

	UPROPERTY(VisibleAnywhere)
	URadarDetectorComponent* radarDetector;

	UPROPERTY()
	FTimerHandle scanForTrackingMissilesHandle;

	UPROPERTY(VisibleAnywhere)
	int trackingMissilesNum = 0;

	UPROPERTY(VisibleAnywhere)
	int trackingFightersNum = 0;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAirPath* FollowedAirPath;

	UPROPERTY(VisibleAnywhere)
	AActor* HomeBase = nullptr;

	virtual void BeginPlay() override;

	void ScanForTrackingThreats();

public:
	virtual void OnPossess(APawn* pawn) override;

	UFUNCTION(BlueprintCallable)
	bool HasValidPath() const;

	UFUNCTION(BlueprintCallable)
	bool IsPathFinished() const;

	UFUNCTION(BlueprintCallable)
	bool IsNearNextWaypoint() const;

	UFUNCTION(BlueprintCallable)
	void SetFollowedPath(UAirPath* path);
	
	UFUNCTION(BlueprintCallable)
	bool FollowNextWaypoint();

	void Tick(float DeltaTime) override;

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	void DrawFollowedPath(float lineLifetime = -1) const;
#endif

	UFUNCTION(BlueprintCallable)
	AActor* GetHomeBase() const;

	UFUNCTION(BlueprintCallable)
	bool SetHomeBase(AActor* airbaseActor);

	virtual void SetGenericTeamId(const FGenericTeamId& team) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void MakePathToLocationAndFollow(const FVector& location);

	UFUNCTION(BlueprintCallable)
	AActor* GetEngagedTarget() const;

	UFUNCTION(BlueprintCallable)
	int GetNumberOfUnitsTargetingActor(AActor* targetActor) const;

	UFUNCTION(BlueprintCallable)
	AActor* SuggestBestTarget(TArray<AActor*> possibleTargets) const;

	UFUNCTION(BlueprintCallable)
	int GetNumMissilesTargetingActor(AActor* actor) const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsTargetEngaged(AActor* targetActor) const override;

	UFUNCTION(BlueprintCallable)
	virtual bool IssueGenericPointOrder(const FVector& location, bool queue = false) override;

	UFUNCTION(BlueprintCallable)
	virtual bool IssueGenericActorOrder(AActor* targetActor, bool queue = false) override;

	UFUNCTION(BlueprintCallable)
	bool CanLandOnBase(const AActor* airbaseActor) const;

	UFUNCTION(BlueprintCallable)
	bool LandOnBase(AActor* airbaseActor);
};
