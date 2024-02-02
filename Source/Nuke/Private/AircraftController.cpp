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

void AAircraftController::ExecuteOrder(UUnitOrder* order)
{
	if (!order)
	{
		return;
	}

	if (UGenericPointOrder* pointOrder = Cast<UGenericPointOrder>(order))
	{
		IssueGenericPointOrder(pointOrder->GetTargetLocation(), false);
	}
	else if (UGenericActorOrder* actorOrder = Cast<UGenericActorOrder>(order))
	{
		IssueGenericActorOrder(actorOrder->GetTargetActor(), false);
	}
	else if (UFollowAirPathOrder* pathOrder = Cast<UFollowAirPathOrder>(order))
	{
		RunBehaviorTree(ScoutLocationTree);
	}
	else if (UDogfightOrder* dogfightOrder = Cast<UDogfightOrder>(order))
	{
		RunBehaviorTree(DogfightTree);
	}
}

void AAircraftController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	radarDetector = pawn->GetComponentByClass<URadarDetectorComponent>();
}

UAirPath* AAircraftController::GetFollowedPath() const
{
	UUnitOrder* order = GetOrderQueueComponent()->GetCurrentOrder();
	UFollowAirPathOrder* followPathOrder = Cast<UFollowAirPathOrder>(order);
	if (!followPathOrder)
	{
		return nullptr;
	}

	return followPathOrder->GetAirPath();
}

bool AAircraftController::HasValidPath() const
{
	return GetFollowedPath() && GetFollowedPath()->IsValid();
}

bool AAircraftController::IsPathFinished() const
{
	check(HasValidPath());
	return GetFollowedPath()->IsFinished();
}

bool AAircraftController::IsNearNextWaypoint() const
{
	if (!HasValidPath())
	{
		return false;
	}

	return GetFollowedPath()->IsNearNextWaypoint(GetPawn()->GetActorLocation());
}

bool AAircraftController::FollowNextWaypoint()
{
	if (!HasValidPath())
	{
		return false;
	}

	if (GetFollowedPath()->IsFinished())
	{
		return false;
	}

	UAircraftMovementComponent* movement = GetPawn<AAircraft>()->GetAircraftMovementComponent();
	movement->RequestFlyTowardsWaypoint(GetFollowedPath()->GetNextWaypoint());
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

	GetFollowedPath()->DebugDrawPath(GetPawn()->GetActorLocation(), lineLifetime);
}
#endif


AActor* AAircraftController::GetHomeBase() const
{
	return HomeBase;
}

bool AAircraftController::SetHomeBase(AActor* airbaseActor)
{
	if (!airbaseActor)
	{
		HomeBase = nullptr;
	}
	else if(Cast<IAirbaseInterface>(airbaseActor))
	{
		HomeBase = airbaseActor;
	}
	else
	{
		return false;
	}

	return true;
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

bool AAircraftController::IssueGenericPointOrder(const FVector& location, bool queue)
{
	if (!ScoutLocationTree)
	{
		UE_LOG(LogTemp, Error, TEXT("ScoutLocationTree not defined!"));
		return false;
	}

	FVector locationCopy = location;
	locationCopy.Z = 2000;

	AAircraft* aircraft = GetPawn<AAircraft>();
	check(aircraft);

	UAircraftMovementComponent* movement = aircraft->GetAircraftMovementComponent();
	float maxSpeed = movement->GetMaxSpeed();

	UUnitOrder* currentOrder = GetOrderQueueComponent()->GetCurrentOrder();

	if (queue)
	{
		if (UFollowAirPathOrder* followPathOrder = Cast<UFollowAirPathOrder>(currentOrder))
		{
			FAircraftWaypoint waypoint(locationCopy, maxSpeed);

			followPathOrder->GetAirPath()->GetPathWaypointsMutable().Add(waypoint);
		}
		else
		{
			TArray<FAircraftWaypoint> pathToLocation;

			pathToLocation.Add(FAircraftWaypoint(locationCopy, maxSpeed));

			UAirPath* pathObject = NewObject<UAirPath>(this, TEXT("AirPath"));
			IssueFollowAirPathOrder(pathObject, true);
		}

		return true;
	}
	
	TArray<FAircraftWaypoint> pathToLocation;

	pathToLocation.Add(FAircraftWaypoint(locationCopy, maxSpeed));

	UAirPath* pathObject = NewObject<UAirPath>(this, TEXT("AirPath"));
	pathObject->SetWaypoints(pathToLocation);

	IssueFollowAirPathOrder(pathObject, false);

	return true;
}

bool AAircraftController::IssueFollowAirPathOrder(UAirPath* path, bool queue)
{
	check(path->IsValid());

	UFollowAirPathOrder* order = NewObject<UFollowAirPathOrder>(this);
	order->SetAirPath(path);
	if (!queue)
	{
		GetOrderQueueComponent()->ClearQueue();		
		GetOrderQueueComponent()->SetCurrentOrder(order);

		RunBehaviorTree(ScoutLocationTree);
		FollowNextWaypoint();
	}
	else
	{
		GetOrderQueueComponent()->Enqueue(order);
	}

	return true;
}

bool AAircraftController::IssueGenericActorOrder(AActor* targetActor, bool queue)
{
	if (IAirbaseInterface* airbase = Cast<IAirbaseInterface>(targetActor))
	{
		bool bHomeBaseOk = SetHomeBase(targetActor);
		if (!bHomeBaseOk)
		{
			return false;
		}

		if (!ReturnToBaseTree)
		{
			UE_LOG(LogTemp, Error, TEXT("ReturnToBaseTree not defined!"));
			return false;
		}

		RunBehaviorTree(ReturnToBaseTree);

		return true;
	}
	else if (UAIUtils::AreEnemies(GetPawn(), targetActor))
	{
		IAttackable* targetAttackable = Cast<IAttackable>(targetActor);
		if (!targetAttackable)
		{
			return false;
		}

		if (targetAttackable->GetMobilityEnvironmentType() == EMobilityEnvironmentType::MET_Air)
		{
			RunBehaviorTree(DogfightTree);
			GetBlackboardComponent()->SetValueAsObject("DogfightTargetAircraft", targetActor);
		}
	}

	return false;
}

void AAircraftController::ReportOrderFinished(bool bSuccessful)
{
	UUnitOrder* nextOrder = GetOrderQueueComponent()->ReportCurrentOrderFinished();
	ExecuteOrder(nextOrder);
}

bool AAircraftController::CanLandOnBase(const AActor* airbaseActor) const
{
	AAircraft* aircraft = GetPawn<AAircraft>();
	check(aircraft);
	const IAirbaseInterface* airbase = Cast<IAirbaseInterface>(airbaseActor);

	return aircraft->CanLandOnBase(airbase);
}

bool AAircraftController::LandOnBase(AActor* airbaseActor)
{
	AAircraft* aircraft = GetPawn<AAircraft>();
	check(aircraft);
	IAirbaseInterface* airbase = Cast<IAirbaseInterface>(airbaseActor);

	if (!aircraft->CanLandOnBase(airbase))
	{
		return false;
	}

	aircraft->LandOnBase(airbase);
	return true;
}

AActor* UDogfightOrder::GetTargetAircraft() const
{
	return targetAircraft;
}

void UFollowAirPathOrder::SetAirPath(UAirPath* path)
{
	this->airPath = path;
}

UAirPath* UFollowAirPathOrder::GetAirPath() const
{
	return airPath;
}

void UDogfightOrder::SetTargetAircraft(AActor* aircraft)
{
	targetAircraft = aircraft;
}

AActor* UReturnToBaseOrder::GetAirbaseActor() const
{
	return airbaseActor;
}

void UReturnToBaseOrder::SetAirbaseActor(AActor* actor)
{
	airbaseActor = actor;
}
