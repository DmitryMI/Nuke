// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftController.h"
#include "Aircraft.h"
#include "AircraftMovementComponent.h"

bool AAircraftController::HasValidPath() const
{
	return FollowedAirPath && FollowedAirPath->IsValid();
}

bool AAircraftController::IsNearNextWaypoint() const
{
	if (!HasValidPath())
	{
		return false;
	}

	return FollowedAirPath->IsNearNextWaypoint(GetPawn()->GetActorLocation());
}

void AAircraftController::SetFollowedPath(UAirPath* path)
{
	FollowedAirPath = path;
}

bool AAircraftController::FollowNextWaypoint()
{
	if (!HasValidPath())
	{
		return false;
	}

	if (FollowedAirPath->IsFinished())
	{
		return false;
	}

	UAircraftMovementComponent* movement = GetPawn<AAircraft>()->GetAircraftMovementComponent();
	movement->RequestFlyTowardsWaypoint(FollowedAirPath->GetNextWaypoint());
	return true;
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
	if (!GetPawn()->IsSelectedInEditor() && !IsSelectedInEditor())
	{
		//return;
	}

	if (!HasValidPath())
	{
		return;
	}

	FollowedAirPath->DebugDrawPath(GetPawn()->GetActorLocation(), lineLifetime);
}
#endif


AAirbase* AAircraftController::GetHomeBase() const
{
	return HomeBase;
}

void AAircraftController::SetHomeBase(AAirbase* airbase)
{
	HomeBase = airbase;
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

void AAircraftController::MakePathToLocationAndFollow(const FVector& location)
{
	AAircraft* aircraft = GetPawn<AAircraft>();
	check(aircraft);

	UAircraftMovementComponent* movement = aircraft->GetAircraftMovementComponent();
	float maxSpeed = movement->GetMaxSpeed();

	TArray<FAircraftWaypoint> pathToLocation;

	FVector direction = location - aircraft->GetActorLocation();
	float distance = direction.Size();
	float intermediateDistance = FMath::Min(10000.0f, distance / 2.0f);

	direction /= distance;
	FVector intermediatePoint = aircraft->GetActorLocation() + direction * intermediateDistance;
	intermediatePoint.Z = 3000.0f;

	pathToLocation.Add(FAircraftWaypoint(intermediatePoint, maxSpeed));
	pathToLocation.Add(FAircraftWaypoint(location, maxSpeed));

	UAirPath* pathObject = NewObject<UAirPath>(this, TEXT("AirPath"));
	pathObject->SetWaypoints(pathToLocation);

	SetFollowedPath(pathObject);
}

