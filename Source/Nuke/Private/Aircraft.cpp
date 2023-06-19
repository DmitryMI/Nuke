// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft.h"
#include "RadarComponent.h"
#include "AirBase.h"

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
}

// Called when the game starts or when spawned
void AAircraft::BeginPlay()
{
	Super::BeginPlay();
	
	if (!aircraftMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("guidedMovement was nullptr!"));
		aircraftMovement = GetComponentByClass<UAircraftMovementComponent>();
	}
}

void AAircraft::DestroyDelayed()
{
	if (!bIsAlive)
	{
		return;
	}

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

TArray<AActor*> AAircraft::GetTrackedThreats() const
{
	TInlineComponentArray<URadarComponent*> radarComponents;
	//TArray<UActorComponent*> radarComponents = GetComponentsByClass(URadarComponent::StaticClass());
	GetComponents(radarComponents);

	TArray<AActor*> trackedThreats;

	for (URadarComponent* radar : radarComponents)
	{
		radar->GetTrackedThreats(trackedThreats);
	}

	return trackedThreats;
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
