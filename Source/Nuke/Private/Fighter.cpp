// Fill out your copyright notice in the Description page of Project Settings.


#include "Fighter.h"
#include "AntiAirMissileController.h"

void AFighter::OnAntiAirMissileCooldownExpired()
{
	bIsAntiAirMissileReady = true;
}

void AFighter::BeginPlay()
{
	Super::BeginPlay();

	fighterRadar = GetComponentByClass<URadarConeComponent>();
	ensure(fighterRadar);

	fighterRadar->SetTrackingRange(radarTrackingRange);
	fighterRadar->SetVisibilityRange(radarTrackingRange);
	fighterRadar->SetConeAngleDeg(radarConeAngle);

	int32 visibleMobilityFlags = 
		(int)EMobilityEnvironmentType::MET_Air | 
		(int)EMobilityEnvironmentType::MET_Ground |
		(int)EMobilityEnvironmentType::MET_WaterSurface;

	int32 trackableMobilityFlags =
		(int)EMobilityEnvironmentType::MET_Air;

	fighterRadar->SetVisibleMobilityFlags(visibleMobilityFlags);
	fighterRadar->SetTrackableMobilityFlags(trackableMobilityFlags);
}

void AFighter::OnManagedMissileDestroyed(AMissile* sender)
{
	AAntiAirMissile* aaMissile = Cast<AAntiAirMissile>(sender);
	managedAntiAirMissiles.Remove(aaMissile);
}

AFighter::AFighter() : Super()
{

}

AAntiAirMissile* AFighter::LaunchAntiAirMissile(AActor* shootTarget)
{
	if (!antiAirMissileType)
	{
		UE_LOG(LogTemp, Error, TEXT("AntiAirMissileType not set!"));
		return nullptr;
	}

	if (!bIsAntiAirMissileReady)
	{
		return nullptr;
	}

	if (antiAirMissileNumber == 0)
	{
		return nullptr;
	}

	if (!IsAlive())
	{
		return nullptr;
	}

	float offset = 100.0f * ((antiAirMissileNumber % 2) * 2 - 1);
	FVector spawnLocation = GetActorLocation() + GetActorRightVector() * offset;
	spawnLocation += FVector::DownVector * 50.0f;

	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;
	AAntiAirMissile* projectile = GetWorld()->SpawnActor<AAntiAirMissile>(antiAirMissileType, spawnLocation, GetActorRotation(), spawnParams);
	if (!projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn a missile!"));
		return nullptr;
	}

	float currentSpeed = GetAircraftMovementComponent()->GetCurrentSpeed();
	projectile->SetCurrentSpeed(currentSpeed);

	bIsAntiAirMissileReady = false;
	antiAirMissileNumber--;

	managedAntiAirMissiles.Add(projectile);
	projectile->OnMissileDestroyed().AddUObject(this, &AFighter::OnManagedMissileDestroyed);

	AAntiAirMissileController* controller = Cast<AAntiAirMissileController>(projectile->GetController());
	check(controller);
	controller->SetTarget(shootTarget);

	GetWorld()->GetTimerManager().SetTimer(
		antiAirMissileCooldownHandle,
		this,
		&AFighter::OnAntiAirMissileCooldownExpired,
		antiAirMissileCooldown,
		false,
		antiAirMissileCooldown
	);

	UE_LOG(LogTemp, Display, TEXT("%s launches a missile %s to intersept %s"), *GetName(), *projectile->GetName(), *shootTarget->GetName());

	return projectile;
}

const TArray<AAntiAirMissile*>& AFighter::GetManagedAntiAirMissiles() const
{
	return managedAntiAirMissiles;
}

bool AFighter::IsAntiAirMissileReady() const
{
	return bIsAntiAirMissileReady;
}

bool AFighter::HasAntiAirMissiles() const
{
	return antiAirMissileNumber > 0;
}

