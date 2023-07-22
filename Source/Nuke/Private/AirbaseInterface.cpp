// Fill out your copyright notice in the Description page of Project Settings.


#include "AirbaseInterface.h"

// Add default functionality here for any IAirbaseInterface functions that are not pure virtual.

bool IAirbaseInterface::CanAircraftLand(const AAircraft* aircraft) const
{
	return false;
}

bool IAirbaseInterface::LandAircraft(AAircraft* aircraft)
{
	return false;
}
