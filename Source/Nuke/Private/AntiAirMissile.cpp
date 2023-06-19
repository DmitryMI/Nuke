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

bool AAntiAirMissile::GetTrackedThreats(TArray<AActor*>& outThreats) const
{
	return false;
}

bool AAntiAirMissile::IsActorTrackedByRadar(AActor* actor) const
{
	if (actor == nullptr)
	{
		return false;
	}

	if (GetLockedOnTarget() != actor)
	{
		return false;
	}

	IAttackable* attackable = Cast<IAttackable>(actor);
	if (!attackable || !attackable->IsAlive())
	{
		return false;
	}

	FVector direction = actor->GetActorLocation() - GetActorLocation();
	double distanceSq = direction.SizeSquared();
	if (distanceSq > FMath::Square(radarRange))
	{
		return false;
	}

	FRotator rotation = GetActorRotation();
	FRotator angleToActor = direction.Rotation();

	// Incorrect, but fast
	double yawDeltaAngleDeg = FMath::FindDeltaAngleDegrees(rotation.Yaw, angleToActor.Yaw);
	double pitchDeltaAngleDeg = FMath::FindDeltaAngleDegrees(rotation.Pitch, angleToActor.Pitch);

	return yawDeltaAngleDeg <= radarConeAngle && pitchDeltaAngleDeg < radarConeAngle;
}

bool AAntiAirMissile::IsTargetTrackedByMissileOrInstigator() const
{
	AActor* target = GetLockedOnTarget();
	if (!target)
	{
		return false;
	}

	bool trackedByMissile = IsActorTrackedByRadar(target);
	if (trackedByMissile)
	{
		return true;
	}

	IRadar* instigatorRadar = Cast<IRadar>(GetInstigator());
	if (!instigatorRadar)
	{
		return false;
	}

	return instigatorRadar->IsActorTrackedByRadar(target);
}
