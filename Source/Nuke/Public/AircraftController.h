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
#include "UnitController.h"
#include "AirbaseInterface.h"
#include "AircraftController.generated.h"

class AAirbase;

UCLASS()
class NUKE_API UDogfightOrder : public UUnitOrder
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	AActor* targetAircraft;

public:
	UFUNCTION(BlueprintCallable)
	void SetTargetAircraft(AActor* aircraft);

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetAircraft() const;
};

UCLASS()
class NUKE_API UFollowAirPathOrder : public UUnitOrder
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UAirPath* airPath;

public:
	UFUNCTION(BlueprintCallable)
	void SetAirPath(UAirPath* path);

	UFUNCTION(BlueprintCallable)
	UAirPath* GetAirPath() const;
};

UCLASS()
class NUKE_API UReturnToBaseOrder : public UFollowAirPathOrder
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	AActor* airbaseActor;

public:
	AActor* GetAirbaseActor() const;

	void SetAirbaseActor(AActor* actor);
};

/**
 * 
 */
UCLASS()
class NUKE_API AAircraftController : public AUnitController, public ICooperativeController
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
	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* ScoutLocationTree;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* DogfightTree;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* ReturnToBaseTree;

	UPROPERTY(VisibleAnywhere)
	AActor* HomeBase = nullptr;

	virtual void BeginPlay() override;

	void ScanForTrackingThreats();

	void ExecuteOrder(UUnitOrder* order);

public:
	virtual void OnPossess(APawn* pawn) override;

	UFUNCTION(BlueprintCallable)
	UAirPath* GetFollowedPath() const;

	UFUNCTION(BlueprintCallable)
	bool HasValidPath() const;

	UFUNCTION(BlueprintCallable)
	bool IsPathFinished() const;

	UFUNCTION(BlueprintCallable)
	bool IsNearNextWaypoint() const;
	
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
	virtual bool IssueFollowAirPathOrder(UAirPath* path, bool queue = false);

	UFUNCTION(BlueprintCallable)
	virtual bool IssueGenericActorOrder(AActor* targetActor, bool queue = false) override;

	virtual void ReportOrderFinished(bool bSuccessful);

	UFUNCTION(BlueprintCallable)
	bool CanLandOnBase(const AActor* airbaseActor) const;

	UFUNCTION(BlueprintCallable)
	bool LandOnBase(AActor* airbaseActor);
};
