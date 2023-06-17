// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AircraftWaypoint.h"
#include "AircraftController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AAircraftController : public AAIController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PathFollowingTolerance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAircraftWaypoint> FollowedPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FollowedPathWaypointIndex = 0;

public:
	UFUNCTION(BlueprintCallable)
	bool IsNearNextWaypoint() const;

	UFUNCTION(BlueprintCallable)
	bool SetFollowedPath(const TArray<FAircraftWaypoint>& path);
	
	UFUNCTION(BlueprintCallable)
	bool FollowNextWaypoint();

	UFUNCTION(BlueprintCallable)
	void MarkNextWaypointAsReached();

	void Tick(float DeltaTime) override;

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	void DrawFollowedPath(float lineLifetime = -1) const;
#endif
};
