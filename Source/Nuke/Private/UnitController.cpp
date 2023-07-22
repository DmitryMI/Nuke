// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitController.h"

bool AUnitController::IssueGenericPointOrder(const FVector& location, bool queue)
{
	return false;
}

bool AUnitController::IssueGenericActorOrder(AActor* actor, bool queue)
{
	return false;
}

bool AUnitController::IssueStopOrder()
{
	return false;
}

void AUnitController::ReportOrderFinished(bool bSuccessful)
{
	IssueStopOrder();
}
