// Fill out your copyright notice in the Description page of Project Settings.


#include "AirbaseController.h"

void AAirbaseController::ScoutLocation(const FVector& location)
{

}

void AAirbaseController::SetGenericTeamId(const FGenericTeamId& team)
{
	APawn* pawn = GetPawn();
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(pawn);
	if (teamAgent)
	{
		teamAgent->SetGenericTeamId(team);
	}

	Super::SetGenericTeamId(team);
}

FGenericTeamId AAirbaseController::GetGenericTeamId() const
{
	APawn* pawn = GetPawn();
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(pawn);
	if (teamAgent)
	{
		return teamAgent->GetGenericTeamId();
	}

	return Super::GetGenericTeamId();
}