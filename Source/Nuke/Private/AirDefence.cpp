// Fill out your copyright notice in the Description page of Project Settings.


#include "AirDefence.h"
#include "AIController.h"
#include "BallisticMissile.h"
#include "SurfaceToAirMissileController.h"

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
	APawn* projectile = GetWorld()->SpawnActor<APawn>(ammoType, GetActorLocation() + FVector::UpVector * 100, FRotator(90, 0, 0), spawnParams);
	if (!projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn SAM!"));
		return nullptr;
	}

	ASurfaceToAirMissileController* controller = Cast<ASurfaceToAirMissileController>(projectile->GetController());
	check(controller);

	controller->SetTarget(target);

	UE_LOG(LogTemp, Display, TEXT("%s launches SAM %s to intersept %s"), *GetName(), *projectile->GetName(), *target->GetName());

	bIsWeaponReady = false;
	GetWorld()->GetTimerManager().SetTimer(
		cooldownHandle,
		this,
		&AAirDefence::OnCooldownExpired,
		weaponCooldown,
		false,
		weaponCooldown
	);

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
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (!teamAgent)
	{
		return;
	}

	if (OtherComp->GetCollisionProfileName() != "Missile")
	{
		return;
	}

	FGenericTeamId otherTeam = teamAgent->GetGenericTeamId();
	if (FGenericTeamId::GetAttitude(GetGenericTeamId(), otherTeam) != ETeamAttitude::Hostile)
	{
		return;
	}

	ABallisticMissile* icbm = Cast<ABallisticMissile>(OtherActor);
	if (icbm && icbm->IsAlive())
	{
		UE_LOG(LogTemp, Display, TEXT("%s now tracks %s"), *GetName(), *OtherActor->GetName());
		trackedEnemies.Add(OtherActor);
	}
	
}

void AAirDefence::OnRadarOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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

	if (trackedEnemies.Contains(OtherActor))
	{
		UE_LOG(LogTemp, Display, TEXT("%s lost track on %s"), *GetName(), *OtherActor->GetName());
		trackedEnemies.Remove(OtherActor);
	}
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
	TArray<AActor*> aliveEnemies;
	for (AActor* actor : trackedEnemies)
	{
		ABallisticMissile* missile = Cast<ABallisticMissile>(actor);
		if (missile->IsAlive())
		{
			aliveEnemies.Add(missile);
		}
	}
	return aliveEnemies;
}

float AAirDefence::GetWeaponMaxSpeed() const
{
	return weaponMaxSpeed;
}

float AAirDefence::GetWeaponAcceleration() const
{
	return weaponAcceleration;
}


