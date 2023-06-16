// Fill out your copyright notice in the Description page of Project Settings.


#include "BallisticMissile.h"
#include "BallisticMovementComponent.h"

void ABallisticMissile::OnDestructionDelayExpired()
{
	Destroy();
}

void ABallisticMissile::OnLifetimeExpired()
{
	Detonate();
}

void ABallisticMissile::OnInvulnerabilityExpired()
{
	SetActorEnableCollision(true);
}

// Sets default values
ABallisticMissile::ABallisticMissile()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent->RegisterComponent();

	bodyCollider = CreateDefaultSubobject<UCapsuleComponent>("BodyColliderComponent", true);
	bodyCollider->SetCapsuleRadius(bodyRadius);
	bodyCollider->SetCapsuleHalfHeight(bodyLength / 2);
	bodyCollider->SetRelativeRotation(FRotator(-90, 0, 0));
	bodyCollider->SetCollisionProfileName("Pawn");
	bodyCollider->SetupAttachment(RootComponent);
	//bodyCollider->RegisterComponent();

	ballisticMovement = CreateDefaultSubobject<UBallisticMovementComponent>(TEXT("BallisticMovementComponent"));

	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ABallisticMissile::BeginPlay()
{
	Super::BeginPlay();

	if (!bodyCollider)
	{
		UE_LOG(LogTemp, Warning, TEXT("bodyCollider was nullptr!"));
		bodyCollider = GetComponentByClass<UCapsuleComponent>();
	}

	if (!ballisticMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("ballisticMovement was nullptr!"));
		ballisticMovement = GetComponentByClass<UBallisticMovementComponent>();
	}

	bodyCollider->OnComponentBeginOverlap.AddDynamic(this, &ABallisticMissile::OnBodyCollisionBegin);

	GetWorld()->GetTimerManager().SetTimer(
		invulnerabilityTimerHandle,
		this,
		&ABallisticMissile::OnInvulnerabilityExpired,
		invulnerabilityDuration,
		false,
		invulnerabilityDuration
	);

	FGenericTeamId team = Cast<IGenericTeamAgentInterface>(GetInstigator())->GetGenericTeamId();
	SetGenericTeamId(team);
}

void ABallisticMissile::DestroyDelayed()
{
	// SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsAlive = false;

	UBallisticMovementComponent* movementComponent = Cast<UBallisticMovementComponent>(FindComponentByClass<UBallisticMovementComponent>());
	check(movementComponent);
	movementComponent->FreezeMovement();

	GetWorld()->GetTimerManager().SetTimer(
		destructionTimerHandle,
		this,
		&ABallisticMissile::OnDestructionDelayExpired,
		destructionDelay,
		false,
		destructionDelay
	);
}

void ABallisticMissile::Detonate()
{
	DestroyDelayed();
}

void ABallisticMissile::OnBodyCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator())
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

	UE_LOG(LogTemp, Display, TEXT("ICBM %s Collided with %s"), *GetName(), *OtherActor->GetName());

	float distanceToTarget = (targetLocation - GetActorLocation()).SizeSquared();
	if (distanceToTarget <= targetTriggerDistance * targetTriggerDistance)
	{
		Detonate();
	}
	else
	{
		DestroyDelayed();
	}
}

// Called every frame
void ABallisticMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallisticMissile::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	teamId = TeamID;
}

FGenericTeamId ABallisticMissile::GetGenericTeamId() const
{
	return teamId;
}

void ABallisticMissile::SetTargetAndLaunchLocation(const FVector& target, const FVector& start)
{
	targetLocation = target;

	UBallisticMovementComponent* movementComponent = Cast<UBallisticMovementComponent>(FindComponentByClass<UBallisticMovementComponent>());
	check(movementComponent);
	movementComponent->SetTargetAndLaunchLocation(target, start);
}

bool ABallisticMissile::IsAlive() const
{
	return bIsAlive;
}

bool ABallisticMissile::PredictLocation(float time, FVector& outLocation) const
{
	return ballisticMovement->PredictLocation(time, outLocation);
}

UBallisticMovementComponent* ABallisticMissile::GetBallisticMovement() const
{
	return ballisticMovement;
}

void ABallisticMissile::ReceiveDamage(float damage)
{
	check(damage >= 0);
	health -= damage;

	if (health <= 0)
	{
		health = 0;
		DestroyDelayed();
	}
}


