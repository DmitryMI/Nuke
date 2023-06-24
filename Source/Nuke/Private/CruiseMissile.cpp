// Fill out your copyright notice in the Description page of Project Settings.


#include "CruiseMissile.h"

ACruiseMissile::ACruiseMissile() : Super()
{
	movementComponent = CreateDefaultSubobject<UAircraftMovementComponent>(TEXT("CruiseMissileMovement"));

}

void ACruiseMissile::BeginPlay()
{
	Super::BeginPlay();

	if (!movementComponent)
	{
		movementComponent = GetComponentByClass<UAircraftMovementComponent>();
	}
}

void ACruiseMissile::SetCurrentSpeed(float speed)
{
	movementComponent->SetCurrentSpeed(speed);
}
