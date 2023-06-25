// Fill out your copyright notice in the Description page of Project Settings.


#include "AirDefenceController.h"
#include "AirDefence.h"
#include "PlaytimeGameState.h"
#include "PlaytimePlayerController.h"
#include "AircraftController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIUtils.h"

bool AAirDefenceController::GetEngagedTargets(TArray<AActor*>& outTargets) const
{
	AAirDefence* airDefence = Cast<AAirDefence>(GetPawn());
	if (!airDefence)
	{
		return false;
	}
	for (AAntiAirMissile* missile : airDefence->GetManagedMissiles())
	{
		if (!missile->IsAlive())
		{
			continue;
		}

		AActor* target = missile->GetLockedOnTarget();
		if (target == nullptr)
		{
			continue;
		}
		outTargets.Add(target);
	}

	return outTargets.Num() > 0;
}

AActor* AAirDefenceController::SuggestBestTarget(TArray<AActor*> possibleTargets) const
{
	if (possibleTargets.Num() == 0)
	{
		return nullptr;
	}

	for (AActor* actor : possibleTargets)
	{
		int missilesEngaged = GetNumOwnMissilesTargetingActor(actor);
		if (missilesEngaged >= 2)
		{
			continue;
		}

		int alliesEngaged = GetNumberOfUnitsTargetingActor(actor);
		
		if (alliesEngaged > 0)
		{
			continue;
		}

		return actor;
	}

	return possibleTargets[0];
}

int AAirDefenceController::GetNumOwnMissilesTargetingActor(AActor* actor) const
{
	int counter = 0;
	if (AAirDefence* airDefence = Cast<AAirDefence>(GetPawn()))
	{
		for (AAntiAirMissile* missile : airDefence->GetManagedMissiles())
		{
			if (missile->GetLockedOnTarget() == actor && missile->IsAlive())
			{
				counter++;
			}
		}
	}
	return counter;
}

int AAirDefenceController::GetNumberOfUnitsTargetingActor(AActor* targetActor) const
{
	AAirDefenceController* self = const_cast<AAirDefenceController*>(this);
	return UAIUtils::GetNumberOfUnitsTargetingActor(self, targetActor);
}

bool AAirDefenceController::IsTargetEngaged(AActor* targetActor) const
{
	AAirDefence* airDefence = Cast<AAirDefence>(GetPawn());
	if (!airDefence)
	{
		return false;
	}
	for (AAntiAirMissile* missile : airDefence->GetManagedMissiles())
	{
		if (!missile->IsAlive())
		{
			continue;
		}

		AActor* target = missile->GetLockedOnTarget();
		if (target == targetActor)
		{
			return true;
		}
	}

	return false;
}

void AAirDefenceController::SetGenericTeamId(const FGenericTeamId& team)
{
	Super::SetGenericTeamId(team);

	IGenericTeamAgentInterface* teamAgent = GetPawn<IGenericTeamAgentInterface>();
	if (!teamAgent)
	{
		return;
	}
}

FGenericTeamId AAirDefenceController::GetGenericTeamId() const
{
	IGenericTeamAgentInterface* teamAgent = GetPawn<IGenericTeamAgentInterface>();
	if (!teamAgent)
	{
		return Super::GetGenericTeamId();
	}

	return teamAgent->GetGenericTeamId();
}
