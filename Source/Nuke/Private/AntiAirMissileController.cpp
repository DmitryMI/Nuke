// Fill out your copyright notice in the Description page of Project Settings.


#include "AntiAirMissileController.h"
#include "AntiAirMissile.h"
#include "AIUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GuidedMissileMovementComponent.h"
#include "DrawDebugHelpers.h"

UGuidedMissileMovementComponent* AAntiAirMissileController::GetGuidedMovementComponent() const
{
	AAntiAirMissile* missile = Cast<AAntiAirMissile>(GetPawn());
	check(missile);

	UGuidedMissileMovementComponent* guidedMissileMovement = missile->FindComponentByClass<UGuidedMissileMovementComponent>();;
	return guidedMissileMovement;
}

void AAntiAirMissileController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAntiAirMissileController::FlyTowardsLocation(const FVector& location)
{
	UGuidedMissileMovementComponent* guidedMissileMovement = GetGuidedMovementComponent();
	check(guidedMissileMovement);

	guidedMissileMovement->RequestMovementTowardsLocation(location);
}

void AAntiAirMissileController::FlyForward()
{
	APawn* pawn = GetPawn();
	FVector location = pawn->GetActorLocation() + pawn->GetActorForwardVector() * 100000.0f;
	FlyTowardsLocation(location);
}

void AAntiAirMissileController::FlyTowardsTargetActor()
{
	UGuidedMissileMovementComponent* guidedMissileMovement = GetGuidedMovementComponent();
	check(guidedMissileMovement);

	guidedMissileMovement->RequestMovementTowardsActor(target);
}

void AAntiAirMissileController::FlyTowardsLinearInterceptionLocation()
{
	if (!GetPawn())
	{
		return;
	}

	UGuidedMissileMovementComponent* guidedMissileMovement = GetGuidedMovementComponent();
	float maxSpeed = guidedMissileMovement->GetMaxSpeed();
	FVector targetLocation = target->GetActorLocation();

	if (IsSelectedInEditor() || GetPawn()->IsSelectedInEditor())
	{
		DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), targetLocation, FColor::Red, false);
	}

	UMovementComponent* targetMovementComponent = target->FindComponentByClass<UMovementComponent>();
	if (targetMovementComponent == nullptr)
	{
		FlyTowardsTargetActor();
		return;
	}

	FVector targetVelocity = targetMovementComponent->Velocity / GetWorld()->DeltaTimeSeconds;
	FVector outPoint;
	
	bool ok = UAIUtils::CalculateInterceptionLinear(
		targetLocation,
		targetVelocity,
		GetPawn()->GetActorLocation(),
		maxSpeed,
		outPoint);

	if (ok)
	{
		if (IsSelectedInEditor() || GetPawn()->IsSelectedInEditor())
		{
			DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), outPoint, FColor::Green, false);
		}
		
		FlyTowardsLocation(outPoint);
	}
	else
	{

		FlyTowardsTargetActor();
	}
}

AActor* AAntiAirMissileController::GetTarget() const
{
	return target;
}

void AAntiAirMissileController::SetTarget(AActor* targetNew)
{
	this->target = targetNew;
	GetBlackboardComponent()->SetValueAsObject("Target", targetNew);

	FlyTowardsTargetActor();
}

void AAntiAirMissileController::SetRendezvousLocation(const FVector& location)
{
	rendezvousLocation = location;
	GetBlackboardComponent()->SetValueAsVector("RendezvousLocation", location);
	GetBlackboardComponent()->SetValueAsBool("HasRendezvousLocation", true);
}

bool AAntiAirMissileController::IsTargetTrackedByMissileOrInstigator() const
{
	AAntiAirMissile* missile = Cast<AAntiAirMissile>(GetPawn());
	check(missile);

	return missile->IsTargetTrackedByMissileOrInstigator();
}
