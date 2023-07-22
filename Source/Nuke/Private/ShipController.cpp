// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipController.h"
#include "Ship.h"
#include "BehaviorTree/BlackboardComponent.h"

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

bool AShipController::IssueGenericPointOrder(const FVector& location, bool queue)
{
	if (!HoldPositionTree)
	{
		UE_LOG(LogTemp, Error, TEXT("ScoutLocationTree not defined!"));
	}
	
	RunBehaviorTree(ScoutLocationTree);

	GetBlackboardComponent()->SetValueAsVector("Location", location);

	return true;
}

bool AShipController::IssueGenericActorOrder(AActor* actor, bool queue)
{
	return false;
}

bool AShipController::IssueStopOrder()
{
	if (!HoldPositionTree)
	{
		UE_LOG(LogTemp, Error, TEXT("HoldPositionTree not defined!"));
		return false;
	}
	RunBehaviorTree(HoldPositionTree);
	return true;
}
