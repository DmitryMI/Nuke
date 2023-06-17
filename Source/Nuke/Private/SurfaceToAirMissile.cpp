// Fill out your copyright notice in the Description page of Project Settings.


#include "SurfaceToAirMissile.h"
#include "Components/ArrowComponent.h"
#include "GuidedMissileMovementComponent.h"
#include "SurfaceToAirMissileController.h"

// Sets default values
ASurfaceToAirMissile::ASurfaceToAirMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	guidedMovement = CreateDefaultSubobject<UGuidedMissileMovementComponent>(TEXT("GuidedMovementComponent"));
}

// Called when the game starts or when spawned
void ASurfaceToAirMissile::BeginPlay()
{
	Super::BeginPlay();

	if (!guidedMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("guidedMovement was nullptr!"));
		guidedMovement = GetComponentByClass<UGuidedMissileMovementComponent>();
	}

	
}

void ASurfaceToAirMissile::OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBodyCollisionBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ASurfaceToAirMissile::OnProximityFuseCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnProximityFuseCollisionBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}



// Called every frame
void ASurfaceToAirMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ASurfaceToAirMissile::SetLockedOnTarget(AActor* actor)
{
	ASurfaceToAirMissileController* controller = GetController<ASurfaceToAirMissileController>();
	controller->SetTarget(actor);
}

AActor* ASurfaceToAirMissile::GetLockedOnTarget() const
{
	ASurfaceToAirMissileController* controller = GetController<ASurfaceToAirMissileController>();
	return controller->GetTarget();
}

void ASurfaceToAirMissile::DestroyDelayed()
{
	Super::DestroyDelayed();

	UGuidedMissileMovementComponent* movementComponent = Cast<UGuidedMissileMovementComponent>(FindComponentByClass<UGuidedMissileMovementComponent>());
	check(movementComponent);
	movementComponent->SetIsEngineOn(false);
	movementComponent->FreezeMovement();
}


void ASurfaceToAirMissile::SetRendezvousLocation(const FVector& location)
{
	ASurfaceToAirMissileController* controller = GetController<ASurfaceToAirMissileController>();
	check(controller);
	controller->SetRendezvousLocation(location);
}

float ASurfaceToAirMissile::GetMaxSpeed() const
{
	return guidedMovement->GetMaxSpeed();
}

float ASurfaceToAirMissile::GetAcceleration() const
{
	return guidedMovement->GetAcceleration();
}
