// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipController.h"
#include "Ship.h"

AShipController::AShipController() : Super()
{
	
}

void AShipController::BeginPlay()
{
	Super::BeginPlay();
	//FNavAgentProperties& navAgentProps = GetNavAgentPropertiesRef();
}

FGenericTeamId AShipController::GetGenericTeamId() const
{
	AShip* ship = GetPawn<AShip>();
	if (!ship)
	{
		return FGenericTeamId::NoTeam;
	}

	return ship->GetGenericTeamId();
}

void AShipController::SetGenericTeamId(const FGenericTeamId& team)
{
	AShip* ship = GetPawn<AShip>();
	if (!ship)
	{
		return;
	}

	ship->SetGenericTeamId(team);
}
