// Fill out your copyright notice in the Description page of Project Settings.


#include "AirPath.h"

UAirPath::UAirPath()
{
}

UAirPath::~UAirPath()
{
}

void UAirPath::SetWaypoints(const TArray<FAircraftWaypoint>& waypoints)
{
	pathWaypoints = waypoints;
	nextWaypointIndex = 0;
}

bool UAirPath::IsValid() const
{
	return pathWaypoints.Num() > 0;
}

bool UAirPath::IsFinished() const
{
	return nextWaypointIndex >= pathWaypoints.Num();
}

const TArray<FAircraftWaypoint>& UAirPath::GetPathWaypoints() const
{
	return pathWaypoints;
}

TArray<FAircraftWaypoint>& UAirPath::GetPathWaypointsMutable()
{
	return pathWaypoints;
}

int UAirPath::GetNextWaypointIndex() const
{
	return nextWaypointIndex;
}

const FAircraftWaypoint& UAirPath::GetNextWaypoint() const
{
	check(IsValid());
	check(!IsFinished());

	return pathWaypoints[nextWaypointIndex];
}

bool UAirPath::IsNearNextWaypoint(const FVector& location) const
{
	float distanceSquared = (GetNextWaypoint().Location - location).SizeSquared();
	return distanceSquared <= FMath::Square(GetNextWaypoint().Tolerance);
}

bool UAirPath::ProcessLocation(const FVector& location)
{
	if (!IsValid())
	{
		return false;
	}

	if (IsFinished())
	{
		return false;
	}

	if (IsNearNextWaypoint(location))
	{
		nextWaypointIndex++;
		return true;
	}

	return false;
}

void UAirPath::DebugDrawPath(const FVector& actorLocation, float lineLifetime) const
{
	if (!IsValid())
	{
		return;
	}

	if (IsFinished())
	{
		return;
	}

	int sphereSegments = 8;
	bool persistentLines = lineLifetime > 0;
	for (int i = 0; i < nextWaypointIndex; i++)
	{
		if (i + 1 >= pathWaypoints.Num())
		{
			break;
		}

		FVector start = pathWaypoints[i].Location;
		FVector end = pathWaypoints[i + 1].Location;

		DrawDebugLine(GetWorld(), start, end, FColor::Green, persistentLines, lineLifetime);
		DrawDebugSphere(GetWorld(), start, pathWaypoints[i].Tolerance, sphereSegments, FColor::Green, persistentLines, lineLifetime);
	}

	DrawDebugSphere(GetWorld(), pathWaypoints[nextWaypointIndex].Location, pathWaypoints[nextWaypointIndex].Tolerance, sphereSegments, FColor::Cyan, persistentLines, lineLifetime);
	DrawDebugLine(GetWorld(), actorLocation, pathWaypoints[nextWaypointIndex].Location, FColor::Cyan, persistentLines, lineLifetime);

	for (int i = nextWaypointIndex; i < pathWaypoints.Num() - 1; i++)
	{
		FVector start = pathWaypoints[i].Location;
		FVector end = pathWaypoints[i + 1].Location;

		DrawDebugLine(GetWorld(), start, end, FColor::Red, persistentLines, lineLifetime);
		DrawDebugSphere(GetWorld(), end, pathWaypoints[i + 1].Tolerance, sphereSegments, FColor::Red, persistentLines, lineLifetime);
	}
}

const FAircraftWaypoint& UAirPath::operator[](int index) const
{
	check(IsValid());
	return pathWaypoints[index];
}

FAircraftWaypoint& UAirPath::operator[](int index)
{
	check(IsValid());
	return pathWaypoints[index];
}
