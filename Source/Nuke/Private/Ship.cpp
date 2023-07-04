// Fill out your copyright notice in the Description page of Project Settings.


#include "Ship.h"
#include "ShipController.h"
#include "PlaytimeUtils.h"

// Sets default values
AShip::AShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	shipMovement = CreateDefaultSubobject<UShipMovementComponent>(TEXT("ShipMovementComponent"));
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();

	if (!shipMovement)
	{
		shipMovement = GetComponentByClass<UShipMovementComponent>();
	}

	APlaytimePlayerState* playerState = UPlaytimeUtils::GetOwningPlayerState(this);
	if (playerState)
	{
		playerState->GetPlayerUnitsMutable().Add(this);
	}
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FGenericTeamId AShip::GetGenericTeamId() const
{
	return teamId;
}

void AShip::SetGenericTeamId(const FGenericTeamId& team)
{
	teamId = team;
}


