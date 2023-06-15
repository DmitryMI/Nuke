// Fill out your copyright notice in the Description page of Project Settings.


#include "SurfaceToAirMissile.h"
#include "Components/ArrowComponent.h"
#include "GuidedMissileMovementComponent.h"
#include "SurfaceToAirMissileController.h"

#if WITH_EDITOR  
void ASurfaceToAirMissile::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "bodyRadius")
	{
		bodyCollider->SetCapsuleRadius(bodyRadius);
	}
	else if (PropertyChangedEvent.Property->GetName() == "bodyLength")
	{
		bodyCollider->SetCapsuleHalfHeight(bodyLength / 2);
	}
	else if (PropertyChangedEvent.Property->GetName() == "proximityFuseTriggerRange")
	{
		proximityCollider->SetSphereRadius(proximityFuseTriggerRange);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ASurfaceToAirMissile::OnDestructionDelayExpired()
{
	Destroy();
}

void ASurfaceToAirMissile::OnLifetimeExpired()
{
	Detonate();
}

// Sets default values
ASurfaceToAirMissile::ASurfaceToAirMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent->RegisterComponent();

	bodyCollider = CreateDefaultSubobject<UCapsuleComponent>("BodyColliderComponent");
	bodyCollider->SetCapsuleRadius(bodyRadius);
	bodyCollider->SetCapsuleHalfHeight(bodyLength / 2);
	bodyCollider->SetCollisionProfileName("Missile");
	bodyCollider->SetRelativeRotation(FRotator(-90, 0, 0));
	bodyCollider->SetupAttachment(RootComponent);	
	
	proximityCollider = CreateDefaultSubobject<USphereComponent>("ProximityColliderComponent");
	proximityCollider->SetSphereRadius(proximityFuseTriggerRange);
	proximityCollider->SetCollisionProfileName("RadarQuery");
	proximityCollider->SetupAttachment(RootComponent);

	guidedMovement = CreateDefaultSubobject<UGuidedMissileMovementComponent>(TEXT("GuidedMovementComponent"));
}

// Called when the game starts or when spawned
void ASurfaceToAirMissile::BeginPlay()
{
	Super::BeginPlay();

	if (!guidedMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("guidedMovement was nullptr!"));
		guidedMovement = GetComponentByClass<UGuidedMissileMovementComponent>();
	}

	bodyCollider->OnComponentBeginOverlap.AddDynamic(this, &ASurfaceToAirMissile::OnBodyCollisionBegin);
	proximityCollider->OnComponentBeginOverlap.AddDynamic(this, &ASurfaceToAirMissile::OnProximityFuseCollisionBegin);
	
	IGenericTeamAgentInterface* teamAgent = GetInstigator<IGenericTeamAgentInterface>();
	if (teamAgent)
	{
		SetGenericTeamId(teamAgent->GetGenericTeamId());
	}

	GetWorld()->GetTimerManager().SetTimer(
		lifetimeTimerHandle,
		this,
		&ASurfaceToAirMissile::OnLifetimeExpired,
		maxLifetime,
		false,
		maxLifetime
	);
}

void ASurfaceToAirMissile::OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

	UE_LOG(LogTemp, Display, TEXT("SAM %s Collided with %s"), *GetName(), *OtherActor->GetName());

	Detonate();
}

void ASurfaceToAirMissile::OnProximityFuseCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASurfaceToAirMissileController* controller = GetController<ASurfaceToAirMissileController>();
	AActor* target = controller->GetTarget();
	if (target == OtherActor)
	{
		Detonate();
	}
}

// Called every frame
void ASurfaceToAirMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ASurfaceToAirMissile::SetLockedOnTarget(AActor* actor)
{
	ASurfaceToAirMissileController* controller = GetController<ASurfaceToAirMissileController>();
	controller->SetTarget(actor);
}

AActor* ASurfaceToAirMissile::GetLockedOnTarget() const
{
	ASurfaceToAirMissileController* controller = GetController<ASurfaceToAirMissileController>();
	return controller->GetTarget();
}

void ASurfaceToAirMissile::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	teamId = TeamID;
}

FGenericTeamId ASurfaceToAirMissile::GetGenericTeamId() const
{
	return teamId;
}

void ASurfaceToAirMissile::DestroyDelayed()
{
	// SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsAlive = false;

	UGuidedMissileMovementComponent* movementComponent = Cast<UGuidedMissileMovementComponent>(FindComponentByClass<UGuidedMissileMovementComponent>());
	check(movementComponent);
	movementComponent->SetIsEngineOn(false);
	movementComponent->FreezeMovement();

	GetWorld()->GetTimerManager().SetTimer(
		destructionTimerHandle,
		this,
		&ASurfaceToAirMissile::OnDestructionDelayExpired,
		destructionDelay,
		false,
		destructionDelay
	);
}

void ASurfaceToAirMissile::Detonate()
{
	if (!bIsAlive)
	{
		return;
	}

	bIsAlive = false;

	FActorSpawnParameters params;
	params.bNoFail = true;
	AExplosion* explosion = GetWorld()->SpawnActor<AExplosion>(explosionType, GetActorLocation(), FRotator::ZeroRotator, params);
	check(explosion);
	explosion->SetBlastDamage(explosionDamageMax);

	DestroyDelayed();
}

bool ASurfaceToAirMissile::IsAlive() const
{
	return bIsAlive;
}

void ASurfaceToAirMissile::SetRendezvousLocation(const FVector& location)
{
	ASurfaceToAirMissileController* controller = GetController<ASurfaceToAirMissileController>();
	check(controller);
	controller->SetRendezvousLocation(location);
}

float ASurfaceToAirMissile::GetMaxSpeed() const
{
	return guidedMovement->GetMaxSpeed();
}

float ASurfaceToAirMissile::GetAcceleration() const
{
	return guidedMovement->GetAcceleration();
}

