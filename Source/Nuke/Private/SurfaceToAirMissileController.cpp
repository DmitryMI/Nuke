// Fill out your copyright notice in the Description page of Project Settings.


#include "SurfaceToAirMissileController.h"
#include "SurfaceToAirMissile.h"
#include "AIUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GuidedMissileMovementComponent.h"
#include "DrawDebugHelpers.h"

UGuidedMissileMovementComponent* ASurfaceToAirMissileController::GetGuidedMovementComponent() const
{
	ASurfaceToAirMissile* missile = Cast<ASurfaceToAirMissile>(GetPawn());
	check(missile);

	UGuidedMissileMovementComponent* guidedMissileMovement = missile->FindComponentByClass<UGuidedMissileMovementComponent>();;
	return guidedMissileMovement;
}

void ASurfaceToAirMissileController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASurfaceToAirMissileController::FlyTowardsLocation(const FVector& location)
{
	UGuidedMissileMovementComponent* guidedMissileMovement = GetGuidedMovementComponent();
	check(guidedMissileMovement);

	guidedMissileMovement->RequestMovementTowardsLocation(location);
}

void ASurfaceToAirMissileController::FlyTowardsTargetActor()
{
	UGuidedMissileMovementComponent* guidedMissileMovement = GetGuidedMovementComponent();
	check(guidedMissileMovement);

	guidedMissileMovement->RequestMovementTowardsTargetActor();
}

void ASurfaceToAirMissileController::FlyTowardsLinearInterceptionLocation()
{
	UGuidedMissileMovementComponent* guidedMissileMovement = GetGuidedMovementComponent();
	float maxSpeed = guidedMissileMovement->GetMaxSpeed();
	FVector targetLocation = target->GetActorLocation();

	DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), targetLocation, FColor::Red, false);

	UMovementComponent* targetMovementComponent = target->FindComponentByClass<UMovementComponent>();
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
		DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), outPoint, FColor::Green, false);

		FlyTowardsLocation(outPoint);
	}
	else
	{
		FlyTowardsTargetActor();
	}
}

AActor* ASurfaceToAirMissileController::GetTarget() const
{
	return target;
}

void ASurfaceToAirMissileController::SetTarget(AActor* targetNew)
{
	this->target = targetNew;
	GetBlackboardComponent()->SetValueAsObject("Target", targetNew);

	//GetPawn<ASurfaceToAirMissile>()->SetLockedOnTarget(targetNew);
	UGuidedMissileMovementComponent* guidedMissileMovement = GetGuidedMovementComponent();
	guidedMissileMovement->SetTargetActor(targetNew);
	FlyTowardsTargetActor();
}

void ASurfaceToAirMissileController::SetRendezvousLocation(const FVector& location)
{
	rendezvousLocation = location;
	GetBlackboardComponent()->SetValueAsVector("RendezvousLocation", location);
}
