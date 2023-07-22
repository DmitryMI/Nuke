// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitControllerInterface.h"

// Add default functionality here for any IControllerMovable functions that are not pure virtual.
bool IUnitControllerInterface::IssueGenericPointOrder(const FVector& location, bool queue)
{
	return false;
}

bool IUnitControllerInterface::IssueGenericActorOrder(AActor* actor, bool queue)
{
	return false;
}

bool IUnitControllerInterface::IssueStopOrder()
{
	return false;
}
