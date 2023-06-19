// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftWaypoint.h"
#include "AirPath.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NUKE_API UAirPath : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TArray<FAircraftWaypoint> pathWaypoints;

	UPROPERTY(EditAnywhere)
	int nextWaypointIndex;

public:
	UAirPath();
	~UAirPath();

	UFUNCTION(BlueprintCallable)
	void SetWaypoints(const TArray<FAircraftWaypoint>& waypoints);

	UFUNCTION(BlueprintCallable)
	bool IsValid() const;

	UFUNCTION(BlueprintCallable)
	bool IsFinished() const;

	UFUNCTION(BlueprintCallable)
	const TArray<FAircraftWaypoint>& GetPathWaypoints() const;

	UFUNCTION(BlueprintCallable)
	TArray<FAircraftWaypoint>& GetPathWaypointsMutable();

	UFUNCTION(BlueprintCallable)
	int GetNextWaypointIndex() const;

	UFUNCTION(BlueprintCallable)
	const FAircraftWaypoint& GetNextWaypoint() const;

	UFUNCTION(BlueprintCallable)
	bool IsNearNextWaypoint(const FVector& location) const;

	UFUNCTION(BlueprintCallable)
	bool ProcessLocation(const FVector& location);

	UFUNCTION(BlueprintCallable)
	void DebugDrawPath(const FVector& actorLocation, float lineLifetime) const;

	const FAircraftWaypoint& operator[](int index) const;
	FAircraftWaypoint& operator[](int index);
};
