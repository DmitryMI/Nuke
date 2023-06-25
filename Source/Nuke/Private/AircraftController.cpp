// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftController.h"
#include "Aircraft.h"
#include "Missile.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AircraftMovementComponent.h"
#include "AIUtils.h"

void AAircraftController::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		scanForTrackingMissilesHandle, 
		this, 
		&AAircraftController::ScanForTrackingThreats,
		scanForTrackingMissilesInterval,
		true, 
		scanForTrackingMissilesInterval
	);
}

void AAircraftController::ScanForTrackingThreats()
{
	AAircraft* aircraft = Cast<AAircraft>(GetPawn());
	check(aircraft);

	if (radarDetector)
	{
		trackingMissilesNum = 0;
		trackingFightersNum = 0;

		TArray<AActor*> radars;
		radarDetector->GetDetectedRadars(radars);
		for (AActor* radar : radars)
		{
			if (Cast<AMissile>(radar))
			{
				trackingMissilesNum++;
			}
			else if (Cast<AAircraft>(radar))
			{
				trackingFightersNum++;
			}
		}
		radarDetector->ClearDetectionData();
	}
}

void AAircraftController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	radarDetector = pawn->GetComponentByClass<URadarDetectorComponent>();
}

bool AAircraftController::HasValidPath() const
{
	return FollowedAirPath && FollowedAirPath->IsValid();
}

bool AAircraftController::IsPathFinished() const
{
	check(HasValidPath());
	return FollowedAirPath->IsFinished();
}

bool AAircraftController::IsNearNextWaypoint() const
{
	if (!HasValidPath())
	{
		return false;
	}

	return FollowedAirPath->IsNearNextWaypoint(GetPawn()->GetActorLocation());
}

void AAircraftController::SetFollowedPath(UAirPath* path)
{
	FollowedAirPath = path;
}

bool AAircraftController::FollowNextWaypoint()
{
	if (!HasValidPath())
	{
		return false;
	}

	if (FollowedAirPath->IsFinished())
	{
		return false;
	}

	UAircraftMovementComponent* movement = GetPawn<AAircraft>()->GetAircraftMovementComponent();
	movement->RequestFlyTowardsWaypoint(FollowedAirPath->GetNextWaypoint());
	return true;
}

void AAircraftController::Tick(float DeltaTime)
{
#if WITH_EDITOR
	DrawFollowedPath();
#endif

	AAircraft* aircraft = Cast<AAircraft>(GetPawn());
	check(aircraft);

	if ((trackingMissilesNum + trackingFightersNum) > 0 && aircraft->HasFlares() && aircraft->AreFlaresReady())
	{
		aircraft->DeployFlareDecoy();
	}
}

#if WITH_EDITOR

void AAircraftController::DrawFollowedPath(float lineLifetime) const
{
	if (!GetPawn()->IsSelectedInEditor() && !IsSelectedInEditor())
	{
		return;
	}

	if (!HasValidPath())
	{
		return;
	}

	FollowedAirPath->DebugDrawPath(GetPawn()->GetActorLocation(), lineLifetime);
}
#endif


AAirbase* AAircraftController::GetHomeBase() const
{
	return HomeBase;
}

void AAircraftController::SetHomeBase(AAirbase* airbase)
{
	HomeBase = airbase;
}

void AAircraftController::SetGenericTeamId(const FGenericTeamId& team)
{
	APawn* pawn = GetPawn();
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(pawn);
	if (teamAgent)
	{
		teamAgent->SetGenericTeamId(team);
	}
	
	Super::SetGenericTeamId(team);
}

FGenericTeamId AAircraftController::GetGenericTeamId() const
{
	APawn* pawn = GetPawn();
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(pawn);
	if (teamAgent)
	{
		return teamAgent->GetGenericTeamId();
	}

	return Super::GetGenericTeamId();
}

void AAircraftController::MakePathToLocationAndFollow(const FVector& location)
{
	AAircraft* aircraft = GetPawn<AAircraft>();
	check(aircraft);

	UAircraftMovementComponent* movement = aircraft->GetAircraftMovementComponent();
	float maxSpeed = movement->GetMaxSpeed();

	TArray<FAircraftWaypoint> pathToLocation;

	FVector direction = location - aircraft->GetActorLocation();
	float distance = direction.Size();
	float intermediateDistance = FMath::Max(10000.0f, distance / 2.0f);

	direction /= distance;
	FVector intermediatePoint = location - direction * intermediateDistance;
	intermediatePoint.Z = 3000.0f;

	pathToLocation.Add(FAircraftWaypoint(intermediatePoint, maxSpeed));
	pathToLocation.Add(FAircraftWaypoint(location, maxSpeed));

	UAirPath* pathObject = NewObject<UAirPath>(this, TEXT("AirPath"));
	pathObject->SetWaypoints(pathToLocation);

	SetFollowedPath(pathObject);
}

AActor* AAircraftController::GetEngagedTarget() const
{
	const UBlackboardComponent* blackboard = GetBlackboardComponent();
	return Cast<AActor>(blackboard->GetValueAsObject("DogfightTargetAircraft"));
}

int AAircraftController::GetNumberOfUnitsTargetingActor(AActor* targetActor) const
{
	AAircraftController* self = const_cast<AAircraftController*>(this);
	return UAIUtils::GetNumberOfUnitsTargetingActor(self, targetActor);
}

AActor* AAircraftController::SuggestBestTarget(TArray<AActor*> possibleTargets) const
{
	if (possibleTargets.Num() == 0)
	{
		return nullptr;
	}

	for (AActor* actor : possibleTargets)
	{
		int alliesEngaged = GetNumberOfUnitsTargetingActor(actor);
		if (alliesEngaged == 0)
		{
			return actor;
		}
	}

	return possibleTargets[0];
}

int AAircraftController::GetNumMissilesTargetingActor(AActor* actor) const
{
	int counter = 0;
	if (AFighter* fighter = Cast<AFighter>(GetPawn()))
	{
		for (AAntiAirMissile* missile : fighter->GetManagedAntiAirMissiles())
		{
			if (missile->GetLockedOnTarget() == actor && missile->IsAlive())
			{
				counter++;
			}
		}
	}
	return counter;
}

bool AAircraftController::IsTargetEngaged(AActor* targetActor) const
{
	return targetActor == GetEngagedTarget();
}

