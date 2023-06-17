// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftController.h"
#include "Aircraft.h"
#include "AircraftMovementComponent.h"

bool AAircraftController::IsNearNextWaypoint() const
{
	if (FollowedPath.Num() == 0)
	{
		return false;
	}

	if (FollowedPathWaypointIndex == FollowedPath.Num())
	{
		return false;
	}

	FVector direction = GetPawn()->GetActorLocation() - FollowedPath[FollowedPathWaypointIndex].Location;
	bool isNear = direction.SizeSquared() <= PathFollowingTolerance * PathFollowingTolerance;

	return isNear;
}

bool AAircraftController::SetFollowedPath(const TArray<FAircraftWaypoint>& path)
{
	FollowedPath = path;
	FollowedPathWaypointIndex = 0;

	return true;
}

bool AAircraftController::FollowNextWaypoint()
{
	if (FollowedPathWaypointIndex >= FollowedPath.Num())
	{
		return false;
	}

	UAircraftMovementComponent* movement = GetPawn<AAircraft>()->GetAircraftMovementComponent();
	movement->RequestFlyTowardsWaypoint(FollowedPath[FollowedPathWaypointIndex]);
	return true;
}

void AAircraftController::MarkNextWaypointAsReached()
{
	FollowedPathWaypointIndex++;
}

void AAircraftController::Tick(float DeltaTime)
{
#if WITH_EDITOR
	DrawFollowedPath();
#endif
}

#if WITH_EDITOR

void AAircraftController::DrawFollowedPath(float lineLifetime) const
{
	if (FollowedPath.Num() == 0)
	{
		return;
	}

	if (FollowedPathWaypointIndex == FollowedPath.Num())
	{
		return;
	}

	if (!GetPawn()->IsSelectedInEditor() && !IsSelectedInEditor())
	{
		return;
	}

	int sphereSegments = 8;
	bool persistentLines = lineLifetime > 0;
	for (int i = 0; i < FollowedPathWaypointIndex; i++)
	{
		if (i + 1 >= FollowedPath.Num())
		{
			break;
		}

		FVector start = FollowedPath[i].Location;
		FVector end = FollowedPath[i + 1].Location;

		DrawDebugLine(GetWorld(), start, end, FColor::Green, persistentLines, lineLifetime);
		DrawDebugSphere(GetWorld(), start, PathFollowingTolerance, sphereSegments, FColor::Green, persistentLines, lineLifetime);
	}

	DrawDebugSphere(GetWorld(), FollowedPath[FollowedPathWaypointIndex].Location, PathFollowingTolerance, sphereSegments, FColor::Cyan, persistentLines, lineLifetime);
	DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), FollowedPath[FollowedPathWaypointIndex].Location, FColor::Cyan, persistentLines, lineLifetime);
	
	for (int i = FollowedPathWaypointIndex; i < FollowedPath.Num() - 1; i++)
	{
		FVector start = FollowedPath[i].Location;
		FVector end = FollowedPath[i + 1].Location;

		DrawDebugLine(GetWorld(), start, end, FColor::Red, persistentLines, lineLifetime);
		//DrawDebugSphere(GetWorld(), start, PathFollowingTolerance, 6, FColor::Red, persistentLines, lineLifetime);
		DrawDebugSphere(GetWorld(), end, PathFollowingTolerance, 8, FColor::Red, persistentLines, lineLifetime);
	}
}

#endif