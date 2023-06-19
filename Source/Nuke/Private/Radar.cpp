// Fill out your copyright notice in the Description page of Project Settings.


#include "Radar.h"

// Add default functionality here for any IRadar functions that are not pure virtual.

bool IRadar::GetTrackedThreats(TArray<AActor*>& outThreats) const
{
	return false;
}

bool IRadar::IsActorTrackedByRadar(AActor* actor) const
{
	return false;
}
