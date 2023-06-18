#include "AircraftWaypoint.h"

FAircraftWaypoint::FAircraftWaypoint()
{
	bIsValid = false;
	Location = FVector::ZeroVector;
	Speed = 0;
}


FAircraftWaypoint::FAircraftWaypoint(const FVector& location, float speed)
{
	bIsValid = true;
	Location = location;
	Speed = speed;
}
