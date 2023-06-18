// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftController.h"
#include "Aircraft.h"
#include "AircraftMovementComponent.h"

bool AAircraftController::HasValidPath() const
{
	if (FollowedPath.Num() == 0)
	{
		return false;
	}

	return FollowedPathWaypointIndex < FollowedPath.Num();
}

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
		//return;
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
		DrawDebugSphere(GetWorld(), end, PathFollowingTolerance, sphereSegments, FColor::Red, persistentLines, lineLifetime);
	}
}
#endif


AAirbase* AAircraftController::GetHomeBase() const
{
	return homeBase;
}

void AAircraftController::SetHomeBase(AAirbase* airbase)
{
	homeBase = airbase;
}

void AAircraftController::ScoutLocation(const FVector& location)
{
	AAircraft* aircraft = GetPawn<AAircraft>();
	check(aircraft);

	UAircraftMovementComponent* movement = aircraft->GetAircraftMovementComponent();
	float maxSpeed = movement->GetMaxSpeed();

	TArray<FAircraftWaypoint> pathToLocation;

	FVector direction = location - aircraft->GetActorLocation();
	float distance = direction.Size();
	float intermediateDistance = FMath::Min(3000.0f, distance / 2.0f);

	direction /= distance;
	FVector intermediatePoint = aircraft->GetActorLocation() + direction * 10000.0f;
	intermediatePoint.Z = location.Z;

	pathToLocation.Add(FAircraftWaypoint(intermediatePoint, maxSpeed));
	pathToLocation.Add(FAircraftWaypoint(location, maxSpeed));

	SetFollowedPath(pathToLocation);
}

void AAircraftController::SetGenericTeamId(const FGenericTeamId& team)
{
	APawn* pawn = GetPawn();
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(pawn);
	if (teamAgent)
	{
		teamAgent->SetGenericTeamId(team);
	}
	
	Super::SetGenericTeamId(team);
}

FGenericTeamId AAircraftController::GetGenericTeamId() const
{
	APawn* pawn = GetPawn();
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(pawn);
	if (teamAgent)
	{
		return teamAgent->GetGenericTeamId();
	}

	return Super::GetGenericTeamId();
}

