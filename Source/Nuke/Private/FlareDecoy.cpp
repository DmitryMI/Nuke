// Fill out your copyright notice in the Description page of Project Settings.


#include "FlareDecoy.h"

void AFlareDecoy::OnFlareDurationExpired()
{
	onFlareDecoyDestroyed.Broadcast(this);
	Destroy();
}

// Sets default values
AFlareDecoy::AFlareDecoy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	flareMovement = CreateDefaultSubobject<UFlareMovementComponent>(TEXT("FlareMovementComponent"));
}

// Called when the game starts or when spawned
void AFlareDecoy::BeginPlay()
{
	Super::BeginPlay();

	if (!flareMovement)
	{
		flareMovement = GetComponentByClass<UFlareMovementComponent>();
	}

	GetWorld()->GetTimerManager().SetTimer(
		flareDurationHandle,
		this, 
		&AFlareDecoy::OnFlareDurationExpired,
		flareDuration,
		false, 
		flareDuration
	);
	
}

// Called every frame
void AFlareDecoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlareDecoy::SetFlareDuration(float duration)
{
	flareDuration = duration;

	float timeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(flareDurationHandle);
	float timeNew = duration - timeRemaining;
	GetWorld()->GetTimerManager().SetTimer(
		flareDurationHandle,
		this,
		&AFlareDecoy::OnFlareDurationExpired,
		timeNew,
		false,
		timeNew
	);
}

float AFlareDecoy::GetFlareDuration() const
{
	return flareDuration;
}

float AFlareDecoy::GetFlareTimeLeft() const
{
	float timeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(flareDurationHandle);
	return timeRemaining;
}

void AFlareDecoy::SetVelocity(const FVector& velocity)
{
	if (!flareMovement)
	{
		flareMovement = GetComponentByClass<UFlareMovementComponent>();
	}
	flareMovement->Velocity = velocity;
}

FFlareDecoyDestroyed& AFlareDecoy::OnFlareDecoyDestroyed()
{
	return onFlareDecoyDestroyed;
}
