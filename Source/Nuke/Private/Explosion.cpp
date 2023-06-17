// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosion.h"
#include "Attackable.h"

#if WITH_EDITOR  
void AExplosion::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "blastRadius")
	{
		blastDamageCollider->SetSphereRadius(blastRadius);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AExplosion::OnBlastExpired()
{
	bIsBlastActive = false;
}

void AExplosion::OnLifetimeExpired()
{
	bIsBlastActive = false;
	Destroy();
}

// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent->RegisterComponent();

	blastDamageCollider = CreateDefaultSubobject<USphereComponent>("BlastDamageColliderComponent", true);
	blastDamageCollider->SetSphereRadius(blastRadius);
	blastDamageCollider->SetCollisionProfileName("Explosion");
	blastDamageCollider->SetupAttachment(RootComponent);
	//bodyCollider->RegisterComponent();

}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(
		lifetimeHandle,
		this,
		&AExplosion::OnLifetimeExpired,
		lifetime,
		false,
		lifetime
	);

	GetWorld()->GetTimerManager().SetTimer(
		blastHandle,
		this,
		&AExplosion::OnBlastExpired,
		blastDuration,
		false,
		blastDuration
	);

	blastDamageCollider->OnComponentBeginOverlap.AddDynamic(this, &AExplosion::OnBlastDamageCollisionBegin);
}

void AExplosion::OnBlastDamageCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bIsBlastActive)
	{
		return;
	}
	IAttackable* attackable = Cast<IAttackable>(OtherActor);
	if (attackable)
	{
		attackable->ReceiveDamage(blastDamageMax);
	}
}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosion::SetBlastDamage(float damage)
{
	blastDamageMax = damage;
}

