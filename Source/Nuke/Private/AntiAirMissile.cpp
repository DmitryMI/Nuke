// Fill out your copyright notice in the Description page of Project Settings.


#include "AntiAirMissile.h"
#include "Components/ArrowComponent.h"
#include "GuidedMissileMovementComponent.h"
#include "AntiAirMissileController.h"

// Sets default values
AAntiAirMissile::AAntiAirMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	guidedMovement = CreateDefaultSubobject<UGuidedMissileMovementComponent>(TEXT("GuidedMovementComponent"));
}

// Called when the game starts or when spawned
void AAntiAirMissile::BeginPlay()
{
	Super::BeginPlay();

	if (!guidedMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("guidedMovement was nullptr!"));
		guidedMovement = GetComponentByClass<UGuidedMissileMovementComponent>();
	}

	
}

void AAntiAirMissile::OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBodyCollisionBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AAntiAirMissile::OnProximityFuseCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnProximityFuseCollisionBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}



// Called every frame
void AAntiAirMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AAntiAirMissile::SetLockedOnTarget(AActor* actor)
{
	AAntiAirMissileController* controller = GetController<AAntiAirMissileController>();
	controller->SetTarget(actor);
}

AActor* AAntiAirMissile::GetLockedOnTarget() const
{
	AAntiAirMissileController* controller = GetController<AAntiAirMissileController>();
	return controller->GetTarget();
}

void AAntiAirMissile::DestroyDelayed()
{
	Super::DestroyDelayed();

	UGuidedMissileMovementComponent* movementComponent = Cast<UGuidedMissileMovementComponent>(FindComponentByClass<UGuidedMissileMovementComponent>());
	check(movementComponent);
	movementComponent->SetIsEngineOn(false);
	movementComponent->FreezeMovement();
}


void AAntiAirMissile::SetRendezvousLocation(const FVector& location)
{
	AAntiAirMissileController* controller = GetController<AAntiAirMissileController>();
	check(controller);
	controller->SetRendezvousLocation(location);
}

float AAntiAirMissile::GetMaxSpeed() const
{
	return guidedMovement->GetMaxSpeed();
}

float AAntiAirMissile::GetAcceleration() const
{
	return guidedMovement->GetAcceleration();
}
