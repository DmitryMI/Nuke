// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft.h"
#include "RadarComponent.h"
#include "AirBase.h"
#include "PlaytimeGameState.h"

#if WITH_EDITOR  

void AAircraft::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "bodyRadius")
	{
		bodyCollider->SetCapsuleRadius(bodyRadius);
	}
	else if (PropertyChangedEvent.Property->GetName() == "bodyLength")
	{
		bodyCollider->SetCapsuleHalfHeight(bodyLength / 2);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif


void AAircraft::OnDestroyDelayedExpired()
{
	Destroy();
}

void AAircraft::OnFlareCooldownExpired()
{
	bFlaresReady = true;
}

// Sets default values
AAircraft::AAircraft()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	bodyCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BodyColliderComponent"));
	bodyCollider->SetCapsuleRadius(bodyRadius);
	bodyCollider->SetCapsuleHalfHeight(bodyLength / 2);
	bodyCollider->SetCollisionProfileName("Pawn");
	bodyCollider->SetRelativeRotation(FRotator(-90, 0, 0));
	bodyCollider->SetupAttachment(RootComponent);

	aircraftMovement = CreateDefaultSubobject<UAircraftMovementComponent>(TEXT("AircraftMovementComponent"));
	radarDetector = CreateDefaultSubobject<URadarDetectorComponent>(TEXT("RadarDetectorComponent"));
	fogOfWarComponent = CreateDefaultSubobject<UFogOfWarComponent>(TEXT("FogOfWar"));

}

// Called when the game starts or when spawned
void AAircraft::BeginPlay()
{
	Super::BeginPlay();

	if (!aircraftMovement)
	{
		aircraftMovement = GetComponentByClass<UAircraftMovementComponent>();
	}

	if (!radarDetector)
	{
		radarDetector = GetComponentByClass<URadarDetectorComponent>();
	}

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

	for (AFlareDecoy* flare : flaresActive)
	{
		flare->OnFlareDecoyDestroyed().AddUObject(this, &AAircraft::OnFlareDecoyDestroyedHandler);
	}
}

void AAircraft::DestroyDelayed()
{
	if (!bIsAlive)
	{
		return;
	}

	APlaytimeGameState* gameState = GetWorld()->GetGameState<APlaytimeGameState>();
	check(gameState);
	APlaytimePlayerState* playerState = gameState->GetPlayerStateByTeam(teamId);
	check(playerState);
	playerState->GetPlayerUnitsMutable().Remove(this);
	
	bIsAlive = false;
	aircraftMovement->FreezeMovement();

	SetActorEnableCollision(false);

	GetWorld()->GetTimerManager().SetTimer(
		destroyDelayedHandle,
		this, 
		&AAircraft::OnDestroyDelayedExpired,
		destroyDelay, 
		false, 
		destroyDelay
	);
}

void AAircraft::OnFlareDecoyDestroyedHandler(AFlareDecoy* sender)
{
	flaresActive.Remove(sender);
}

// Called every frame
void AAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

bool AAircraft::IsAlive() const
{
	return bIsAlive;
}

void AAircraft::ReceiveDamage(float damageAmount)
{
	check(damageAmount >= 0);

	if (!bIsAlive)
	{
		return;
	}

	health -= damageAmount;

	if (health <= 0)
	{
		health = 0;
		DestroyDelayed();
	}
}

void AAircraft::SetGenericTeamId(const FGenericTeamId& team)
{
	teamId = team;
}

FGenericTeamId AAircraft::GetGenericTeamId() const
{
	return teamId;
}

UAircraftMovementComponent* AAircraft::GetAircraftMovementComponent() const
{
	return aircraftMovement;
}

TArray<AActor*> AAircraft::GetTrackedThreatsArray() const
{
	TInlineComponentArray<URadarComponent*> radarComponents;
	GetComponents(radarComponents);

	TArray<AActor*> trackedThreats;

	GetTrackedThreats(trackedThreats);

	return trackedThreats;
}

bool AAircraft::GetTrackedThreats(TArray<AActor*>& outThreats) const
{
	if (!IsAlive())
	{
		return false;
	}

	TInlineComponentArray<URadarComponent*> radarComponents;
	GetComponents(radarComponents);

	for (URadarComponent* radar : radarComponents)
	{
		radar->GetTrackedThreats(outThreats);
	}

	return outThreats.Num() > 0;
}

bool AAircraft::CanLandOnBase(AAirbase* airbase) const
{
	if (!airbase || !airbase->IsAlive())
	{
		return false;
	}

	if (!bIsAlive)
	{
		return false;
	}

	if (FGenericTeamId::GetAttitude(airbase->GetGenericTeamId(), GetGenericTeamId()) != ETeamAttitude::Friendly)
	{
		return false;
	}

	FVector baseLocation = airbase->GetActorLocation();
	FVector aircraftLocation = GetActorLocation();

	return (baseLocation - aircraftLocation).SizeSquared() <= landingDistance * landingDistance;
}

void AAircraft::LandOnBase(AAirbase* airbase)
{
	if (CanLandOnBase(airbase))
	{
		DestroyDelayed();
		airbase->SetNumberOfFighters(airbase->GetNumberOfFighters() + 1);
	}
}

bool AAircraft::IsActorTrackedByRadar(AActor* actor) const
{
	if (!IsAlive())
	{
		return false;
	}

	TInlineComponentArray<URadarComponent*> radarComponents;
	GetComponents(radarComponents);

	for (URadarComponent* radar : radarComponents)
	{
		if (radar->IsActorTrackedByRadar(actor))
		{
			return true;
		}
	}

	return false;
}

EMobilityEnvironmentType AAircraft::GetMobilityEnvironmentType() const
{
	return EMobilityEnvironmentType::MET_Air;
}

AFlareDecoy* AAircraft::DeployFlareDecoy()
{
	if (!IsAlive())
	{
		return nullptr;
	}

	if (!AreFlaresReady() || !HasFlares())
	{
		return nullptr;
	}

	FVector spawnLocation = GetActorLocation();
	FRotator spawnRotation = GetActorRotation();
	FActorSpawnParameters params;
	params.Instigator = this;
	params.Owner = this;
	AFlareDecoy* flare = GetWorld()->SpawnActor<AFlareDecoy>(flareType, spawnLocation, spawnRotation, params);
	check(flare);

	FVector velocity = GetActorRightVector() * ((flaresCharges % 2) * 2 - 1) * 300.0f * GetWorld()->DeltaTimeSeconds;
	velocity += GetAircraftMovementComponent()->Velocity;
	flare->SetVelocity(velocity);

	bFlaresReady = false;
	flaresCharges--;

	GetWorld()->GetTimerManager().SetTimer(
		flareCooldownHandle,
		this,
		&AAircraft::OnFlareCooldownExpired,
		flaresCooldown,
		false,
		flaresCooldown
	);

	flaresActive.Add(flare);
	flare->OnFlareDecoyDestroyed().AddUObject(this, &AAircraft::OnFlareDecoyDestroyedHandler);

	return flare;
}

bool AAircraft::HasFlares() const
{
	return flaresCharges > 0;
}

bool AAircraft::AreFlaresReady() const
{
	return bFlaresReady;
}

const TArray<AFlareDecoy*>& AAircraft::GetActiveFlayers() const
{
	return flaresActive;
}

int AAircraft::GetActiveFlayersCount() const
{
	return flaresActive.Num();
}

float AAircraft::GetStealthFactor() const
{
	return stealthFactor;
}

UFogOfWarComponent* AAircraft::GetFogOfWarComponent() const
{
	check(fogOfWarComponent);
	return fogOfWarComponent;
}
