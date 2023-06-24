// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomber.h"

void ABomber::OnCruiseMissileCooldownExpired()
{
	bIsCruiseMissileReady = true;
}

ACruiseMissile* ABomber::LaunchCruiseMissile(const FVector& targetLocation)
{
	if (!cruiseMissileType)
	{
		UE_LOG(LogTemp, Error, TEXT("CruiseMissileType not set!"));
		return nullptr;
	}

	if (!bIsCruiseMissileReady)
	{
		return nullptr;
	}

	if (cruiseMissileNumber == 0)
	{
		return nullptr;
	}

	if (!IsAlive())
	{
		return nullptr;
	}

	float offset = 100.0f * ((cruiseMissileNumber % 2) * 2 - 1);
	FVector spawnLocation = GetActorLocation() + GetActorRightVector() * offset;
	spawnLocation += FVector::DownVector * 50.0f;

	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;
	ACruiseMissile* projectile = GetWorld()->SpawnActor<ACruiseMissile>(cruiseMissileType, spawnLocation, GetActorRotation(), spawnParams);
	if (!projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn a missile!"));
		return nullptr;
	}

	float currentSpeed = GetAircraftMovementComponent()->GetCurrentSpeed();
	projectile->SetCurrentSpeed(currentSpeed);

	bIsCruiseMissileReady = false;
	cruiseMissileNumber--;

	//AAntiAirMissileController* controller = Cast<AAntiAirMissileController>(projectile->GetController());
	//check(controller);
	//controller->SetTarget(shootTarget);

	GetWorld()->GetTimerManager().SetTimer(
		cruiseMissileCooldownHandle,
		this,
		&ABomber::OnCruiseMissileCooldownExpired,
		cruiseMissileCooldown,
		false,
		cruiseMissileCooldown
	);

	UE_LOG(LogTemp, Display, TEXT("%s launches a cruise missile %s at %s"), *GetName(), *projectile->GetName(), *targetLocation.ToString());

	return projectile;

	return nullptr;
}

bool ABomber::IsCruiseMissileReady() const
{
	return bIsCruiseMissileReady;
}

bool ABomber::HasCruiseMissiles() const
{
	return cruiseMissileNumber > 0;
}
