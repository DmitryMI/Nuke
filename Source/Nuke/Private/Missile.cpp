// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"

#if WITH_EDITOR  

void AMissile::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
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

// Sets default values
AMissile::AMissile()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent->RegisterComponent();

	bodyCollider = CreateDefaultSubobject<UCapsuleComponent>("BodyColliderComponent");
	bodyCollider->SetCapsuleRadius(bodyRadius);
	bodyCollider->SetCapsuleHalfHeight(bodyLength / 2);
	bodyCollider->SetCollisionProfileName("Pawn");
	bodyCollider->SetRelativeRotation(FRotator(-90, 0, 0));
	bodyCollider->SetupAttachment(RootComponent);

	fogOfWarComponent = CreateDefaultSubobject<UFogOfWarComponent>(TEXT("FogOfWar"));
}

void AMissile::OnDestructionDelayExpired()
{
	Destroy();
}

void AMissile::OnLifetimeExpired()
{
	Detonate();
}


// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	
	bodyCollider->OnComponentBeginOverlap.AddDynamic(this, &AMissile::OnBodyOverlapBegin);

	IGenericTeamAgentInterface* teamAgent = GetInstigator<IGenericTeamAgentInterface>();
	if (teamAgent)
	{
		SetGenericTeamId(teamAgent->GetGenericTeamId());
	}

	if (!fogOfWarComponent)
	{
		fogOfWarComponent = GetComponentByClass<UFogOfWarComponent>();
	}

	GetWorld()->GetTimerManager().SetTimer(
		lifetimeTimerHandle,
		this,
		&AMissile::OnLifetimeExpired,
		maxLifetime,
		false,
		maxLifetime
	);
}

void AMissile::OnBodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Display, TEXT("Missile %s (body) collided with %s"), *GetName(), *OtherActor->GetName());

	Detonate();
}

void AMissile::OnBodyOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* instigator = GetInstigator();
	if (OtherActor == instigator)
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

	UE_LOG(LogTemp, Display, TEXT("Missile %s (body) overlaped with %s"), *GetName(), *OtherActor->GetName());

	Detonate();
}

FMissileDestroyedEvent& AMissile::OnMissileDestroyed()
{
	return missileDestroyedEvent;
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//for(AActor* actor : G)
}


void AMissile::DestroyDelayed()
{
	missileDestroyedEvent.Broadcast(this);

	// SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsAlive = false;

	GetWorld()->GetTimerManager().SetTimer(
		destructionTimerHandle,
		this,
		&AMissile::OnDestructionDelayExpired,
		destructionDelay,
		false,
		destructionDelay
	);
}

void AMissile::Detonate()
{
	if (!bIsAlive)
	{
		return;
	}

	bIsAlive = false;

	FActorSpawnParameters params;
	params.bNoFail = true;
	AExplosion* explosion = GetWorld()->SpawnActor<AExplosion>(explosionType, GetActorLocation(), FRotator::ZeroRotator, params);
	if (!explosion)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Explosion!"));
		DestroyDelayed();
		return;
	}

	explosion->SetBlastDamage(explosionDamageMax);

	DestroyDelayed();
}

bool AMissile::IsAlive() const
{
	return bIsAlive;
}

void AMissile::SetGenericTeamId(const FGenericTeamId& team)
{
	teamId = team;
}

FGenericTeamId AMissile::GetGenericTeamId() const
{
	return teamId;
}

float AMissile::GetProximityFuseTriggerRange() const
{
	return proximityFuseTriggerRange;
}

EMobilityEnvironmentType AMissile::GetMobilityEnvironmentType() const
{
	return EMobilityEnvironmentType::MET_Air;
}

float AMissile::GetStealthFactor() const
{
	return 1.0f;
}

UFogOfWarComponent* AMissile::GetFogOfWarComponent() const
{
	return fogOfWarComponent;
}
