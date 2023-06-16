// Fill out your copyright notice in the Description page of Project Settings.


#include "AirDefence.h"
#include "AIController.h"
#include "BallisticMissile.h"
#include "SurfaceToAirMissileController.h"

bool AAirDefence::IsAlive() const
{
	return bIsAlive;
}

void AAirDefence::ReceiveDamage(float damageAmount)
{
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

	radarCollider = CreateDefaultSubobject<USphereComponent>("RadarQueryCollider");
	radarCollider->SetCollisionProfileName(TEXT("RadarQuery"));
	radarCollider->SetSphereRadius(weaponRange);
	radarCollider->SetupAttachment(RootComponent);
	//radarCollider->RegisterComponent();
}

// Called when the game starts or when spawned
void AAirDefence::BeginPlay()
{
	Super::BeginPlay();
	//check(radarCollider);
	radarCollider->OnComponentBeginOverlap.AddDynamic(this, &AAirDefence::OnRadarOverlapBegin);
	radarCollider->OnComponentEndOverlap.AddDynamic(this, &AAirDefence::OnRadarOverlapEnd);

	for (ASurfaceToAirMissile* missile : managedMissiles)
	{
		missile->OnMissileDestroyed().AddUObject(this, &AAirDefence::OnManagedMissileDestroyed);
	}
}

AActor* AAirDefence::Shoot(AActor* target)
{
	if (!bIsWeaponReady)
	{
		return nullptr;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;
	ASurfaceToAirMissile* projectile = GetWorld()->SpawnActor<ASurfaceToAirMissile>(ammoType, GetActorLocation() + FVector::UpVector * 100, FRotator(90, 0, 0), spawnParams);
	if (!projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn SAM!"));
		return nullptr;
	}

	managedMissiles.Add(projectile);
	projectile->OnMissileDestroyed().AddUObject(this, &AAirDefence::OnManagedMissileDestroyed);

	ASurfaceToAirMissileController* controller = Cast<ASurfaceToAirMissileController>(projectile->GetController());
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

void AAirDefence::OnRadarOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IAttackable* attackable = Cast<IAttackable>(OtherActor);
	if (!attackable)
	{
		return;
	}

	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (!teamAgent)
	{
		return;
	}

	if (OtherComp->GetCollisionProfileName() != "Pawn")
	{
		return;
	}

	FGenericTeamId otherTeam = teamAgent->GetGenericTeamId();
	if (FGenericTeamId::GetAttitude(GetGenericTeamId(), otherTeam) != ETeamAttitude::Hostile)
	{
		return;
	}

	if (attackable->IsAlive())
	{
		UE_LOG(LogTemp, Display, TEXT("%s now tracks %s"), *GetName(), *OtherActor->GetName());
		threatsInRadarRange.Add(OtherActor);
	}
	
}

void AAirDefence::OnRadarOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IAttackable* attackable = Cast<IAttackable>(OtherActor);
	if (!attackable)
	{
		return;
	}

	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (!teamAgent)
	{
		return;
	}

	FGenericTeamId otherTeam = teamAgent->GetGenericTeamId();
	if (FGenericTeamId::GetAttitude(GetGenericTeamId(), otherTeam) != ETeamAttitude::Hostile)
	{
		return;
	}

	if (OtherComp->GetCollisionProfileName() != "Pawn")
	{
		return;
	}

	if (threatsInRadarRange.Contains(OtherActor))
	{
		UE_LOG(LogTemp, Display, TEXT("%s lost track on %s"), *GetName(), *OtherActor->GetName());
		threatsInRadarRange.Remove(OtherActor);
	}
}

void AAirDefence::OnManagedMissileDestroyed(ASurfaceToAirMissile* missile)
{
	managedMissiles.Remove(missile);
}


#if WITH_EDITOR  
void AAirDefence::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "weaponRange")
	{
		radarCollider->SetSphereRadius(weaponRange);
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

TArray<AActor*> AAirDefence::GetTrackedEnemies() const
{
	TArray<AActor*> trackedEnemies;
	for (AActor* actor : threatsInRadarRange)
	{
		IAttackable* attackable = Cast<IAttackable>(actor);
		check(attackable);

		if (!attackable->IsAlive())
		{
			continue;
		}

		TArray<FHitResult> hitResults;
		FVector traceStart = GetActorLocation() + FVector::UpVector * 100.0f;
		FVector traceEnd = actor->GetActorLocation();
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(this);
		queryParams.AddIgnoredActor(actor);
		GetWorld()->LineTraceMultiByProfile(hitResults, traceStart, traceEnd, "RadarLineOfSight", queryParams);
		if (hitResults.Num() != 0)
		{
			continue;
		}

		trackedEnemies.Add(actor);
	}
	return trackedEnemies;
}

float AAirDefence::GetWeaponMaxSpeed() const
{
	return weaponMaxSpeed;
}

float AAirDefence::GetWeaponAcceleration() const
{
	return weaponAcceleration;
}

const TArray<ASurfaceToAirMissile*> AAirDefence::GetManagedMissiles() const
{
	return managedMissiles;
}


