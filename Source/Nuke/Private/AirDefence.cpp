// Fill out your copyright notice in the Description page of Project Settings.


#include "AirDefence.h"
#include "AIController.h"
#include "BallisticMissile.h"
#include "PlaytimeGameState.h"
#include "PlaytimePlayerState.h"
#include "AntiAirMissileController.h"

bool AAirDefence::IsAlive() const
{
	return bIsAlive;
}

void AAirDefence::ReceiveDamage(float damageAmount)
{
}

UFogOfWarComponent* AAirDefence::GetFogOfWarComponent() const
{
	return fogOfWarComponent;
}

void AAirDefence::OnCooldownExpired()
{
	bIsWeaponReady = true;
}

void AAirDefence::OnShootDelayExpired()
{
	Shoot(lockedTarget);
	lockedTarget = nullptr;
}

// Sets default values
AAirDefence::AAirDefence()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent->RegisterComponent();

	radarComponent = CreateDefaultSubobject<URadarSphereComponent>("RadarCollider");
	radarComponent->SetTrackingRange(weaponRange);
	radarComponent->SetVisibilityRange(weaponRange);
	radarComponent->SetupAttachment(RootComponent);

	int32 visibleMobilityFlags =
		(int)EMobilityEnvironmentType::MET_Air |
		(int)EMobilityEnvironmentType::MET_Ground |
		(int)EMobilityEnvironmentType::MET_WaterSurface;

	int32 trackableMobilityFlags =
		(int)EMobilityEnvironmentType::MET_Air;

	radarComponent->SetTrackableMobilityFlags(trackableMobilityFlags);
	radarComponent->SetVisibleMobilityFlags(visibleMobilityFlags);

	fogOfWarComponent = CreateDefaultSubobject<UFogOfWarComponent>(TEXT("FogOfWar"));
}

// Called when the game starts or when spawned
void AAirDefence::BeginPlay()
{
	Super::BeginPlay();

	for (AAntiAirMissile* missile : managedMissiles)
	{
		missile->OnMissileDestroyed().AddUObject(this, &AAirDefence::OnManagedMissileDestroyed);
	}

	if (!radarComponent)
	{
		radarComponent = GetComponentByClass<URadarSphereComponent>();
	}

	int32 visibleMobilityFlags =
		(int)EMobilityEnvironmentType::MET_Air |
		(int)EMobilityEnvironmentType::MET_Ground |
		(int)EMobilityEnvironmentType::MET_WaterSurface;

	int32 trackableMobilityFlags =
		(int)EMobilityEnvironmentType::MET_Air;

	radarComponent->SetTrackableMobilityFlags(trackableMobilityFlags);
	radarComponent->SetVisibleMobilityFlags(visibleMobilityFlags);

	if (!fogOfWarComponent)
	{
		fogOfWarComponent = GetComponentByClass<UFogOfWarComponent>();
	}

	if (teamId != FGenericTeamId::NoTeam)
	{
		APlaytimeGameState* gameState = GetWorld()->GetGameState<APlaytimeGameState>();
		check(gameState);
		APlaytimePlayerState* playerState = gameState->GetPlayerStateByTeam(teamId);
		check(playerState);
		playerState->GetPlayerUnitsMutable().Add(this);
	}
}

AActor* AAirDefence::Shoot(AActor* target)
{
	if (!ensure(target))
	{
		return nullptr;
	}

	if (!bIsWeaponReady)
	{
		return nullptr;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;
	AAntiAirMissile* projectile = GetWorld()->SpawnActor<AAntiAirMissile>(ammoType, GetActorLocation() + FVector::UpVector * 100, FRotator(90, 0, 0), spawnParams);
	if (!projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn SAM!"));
		return nullptr;
	}

	managedMissiles.Add(projectile);
	projectile->OnMissileDestroyed().AddUObject(this, &AAirDefence::OnManagedMissileDestroyed);

	AAntiAirMissileController* controller = Cast<AAntiAirMissileController>(projectile->GetController());
	check(controller);
	controller->SetTarget(target);

	bIsWeaponReady = false;
	GetWorld()->GetTimerManager().SetTimer(
		cooldownHandle,
		this,
		&AAirDefence::OnCooldownExpired,
		weaponCooldown,
		false,
		weaponCooldown
	);

	UE_LOG(LogTemp, Display, TEXT("%s launches SAM %s to intersept %s"), *GetName(), *projectile->GetName(), *target->GetName());

	return projectile;
}

void AAirDefence::ShootDelayed(AActor* target, float delay)
{
	lockedTarget = target;

	GetWorld()->GetTimerManager().SetTimer(
		shootDelayHandle,
		this,
		&AAirDefence::OnShootDelayExpired,
		delay,
		false,
		delay
	);
}

void AAirDefence::OnManagedMissileDestroyed(AMissile* missile)
{
	AAntiAirMissile* sam = Cast<AAntiAirMissile>(missile);
	managedMissiles.Remove(sam);
}


#if WITH_EDITOR  
void AAirDefence::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "weaponRange")
	{
		radarComponent->SetTrackingRange(weaponRange);
	}


	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

// Called every frame
void AAirDefence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AAirDefence::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	teamId = TeamID;
}

FGenericTeamId AAirDefence::GetGenericTeamId() const
{
	return teamId;
}

bool AAirDefence::IsWeaponReady() const
{
	return bIsWeaponReady;
}

bool AAirDefence::GetTrackedThreats(TArray<AActor*>& outThreats) const
{
	if (!radarComponent)
	{
		return false;
	}
	return radarComponent->GetTrackedThreats(outThreats);
}

bool AAirDefence::IsActorTrackedByRadar(AActor* actor) const
{
	if (!radarComponent)
	{
		return false;
	}
	return radarComponent->IsActorTrackedByRadar(actor);
}

float AAirDefence::GetWeaponMaxSpeed() const
{
	return weaponMaxSpeed;
}

float AAirDefence::GetWeaponAcceleration() const
{
	return weaponAcceleration;
}

const TArray<AAntiAirMissile*>& AAirDefence::GetManagedMissiles() const
{
	return managedMissiles;
}


