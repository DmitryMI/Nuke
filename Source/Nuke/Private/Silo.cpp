// Fill out your copyright notice in the Description page of Project Settings.


#include "Silo.h"
#include "BallisticMissile.h"

void ASilo::OnCooldownExpired()
{
	bIsWeaponReady = true;
}

// Sets default values
ASilo::ASilo()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASilo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASilo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASilo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

AActor* ASilo::Shoot(const FVector& targetLocation)
{
	if (!bIsWeaponReady)
	{
		return nullptr;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;
	APawn* projectile = GetWorld()->SpawnActor<APawn>(ammoType, GetActorLocation(), FRotator(90, 0, 0), spawnParams);
	if (!projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn ICBM!"));
		return nullptr;
	}

	ABallisticMissile* missile = Cast<ABallisticMissile>(projectile);

	FVector2D scatterAngle = FMath::RandPointInCircle(errorRadius);
	FVector targetLocationWithError = targetLocation + FVector(scatterAngle.X, scatterAngle.Y, 0);

	missile->SetTargetAndLaunchLocation(targetLocationWithError, GetActorLocation());

	bIsWeaponReady = false;
	GetWorld()->GetTimerManager().SetTimer(
		cooldownHandle,
		this,
		&ASilo::OnCooldownExpired,
		weaponCooldown,
		false,
		weaponCooldown
	);

	return projectile;
}

void ASilo::SetGenericTeamId(const FGenericTeamId& team)
{
	teamId = team;
}

FGenericTeamId ASilo::GetGenericTeamId() const
{
	return teamId;
}

