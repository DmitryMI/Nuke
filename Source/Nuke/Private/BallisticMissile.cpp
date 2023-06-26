// Fill out your copyright notice in the Description page of Project Settings.


#include "BallisticMissile.h"
#include "BallisticMovementComponent.h"

// Sets default values
ABallisticMissile::ABallisticMissile() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ballisticMovement = CreateDefaultSubobject<UBallisticMovementComponent>(TEXT("BallisticMovementComponent"));

}

// Called when the game starts or when spawned
void ABallisticMissile::BeginPlay()
{
	Super::BeginPlay();

	if (!ballisticMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("ballisticMovement was nullptr!"));
		ballisticMovement = GetComponentByClass<UBallisticMovementComponent>();
	}
}


void ABallisticMissile::OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator())
	{
		return;
	}

	if (OtherActor == this)
	{
		return;
	}

	FName otherCollisionProfile = OtherComp->GetCollisionProfileName();

	if (otherCollisionProfile == "RadarQuery")
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("ICBM %s Collided with %s"), *GetName(), *OtherActor->GetName());

	float distanceToTarget = (targetLocation - GetActorLocation()).SizeSquared();
	float fuseSquared = FMath::Square(GetProximityFuseTriggerRange());
	if (distanceToTarget <= fuseSquared)
	{
		Detonate();
	}
	else
	{
		DestroyDelayed();
	}
}

// Called every frame
void ABallisticMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallisticMissile::SetTargetAndLaunchLocation(const FVector& target, const FVector& start)
{
	targetLocation = target;

	UBallisticMovementComponent* movementComponent = Cast<UBallisticMovementComponent>(FindComponentByClass<UBallisticMovementComponent>());
	check(movementComponent);
	movementComponent->SetTargetAndLaunchLocation(target, start);
}

bool ABallisticMissile::PredictLocation(float time, FVector& outLocation) const
{
	return ballisticMovement->PredictLocation(time, outLocation);
}

UBallisticMovementComponent* ABallisticMissile::GetBallisticMovement() const
{
	return ballisticMovement;
}

void ABallisticMissile::DestroyDelayed()
{
	Super::DestroyDelayed();
	ballisticMovement->FreezeMovement();
}


