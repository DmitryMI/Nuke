// Fill out your copyright notice in the Description page of Project Settings.


#include "Airbase.h"
#include "Kismet/GameplayStatics.h"
#include "AircraftController.h"

void AAirbase::OnLauchStateCooldownExpired()
{
	bIsLaunchReady = true;
}

// Sets default values
AAirbase::AAirbase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fogOfWarComponent = CreateDefaultSubobject<UFogOfWarComponent>(TEXT("FogOfWar"));
}

// Called when the game starts or when spawned
void AAirbase::BeginPlay()
{
	Super::BeginPlay();

	if (!fogOfWarComponent)
	{
		fogOfWarComponent = GetComponentByClass<UFogOfWarComponent>();
	}
}

bool AAirbase::IsReadyToLaunch() const
{
	return bIsLaunchReady;
}

AFighter* AAirbase::LaunchFigher()
{
	if (fightersNumber == 0)
	{
		return nullptr;
	}

	AAircraft* aircraft = LaunchAircraft(fighterType);
	if (!aircraft)
	{
		return nullptr;
	}

	fightersNumber--;
	return Cast<AFighter>(aircraft);
}

ABomber* AAirbase::LaunchBomber()
{
	if (bombersNumber == 0)
	{
		return nullptr;
	}

	AAircraft* aircraft = LaunchAircraft(bomberType);
	if (!aircraft)
	{
		return nullptr;
	}

	bombersNumber--;
	return Cast<ABomber>(aircraft);
}

AAircraft* AAirbase::LaunchAircraft(TSubclassOf<AAircraft> aircraftType)
{
	if (!bIsLaunchReady)
	{
		return nullptr;
	}

	bIsLaunchReady = false;

	FVector spawnLocation = GetActorLocation() + FVector::UpVector * 100.0f;
	FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;
	FTransform spawnTransform(spawnRotation, spawnLocation, FVector(1, 1, 1));
	AAircraft* aircraft = GetWorld()->SpawnActorDeferred<AAircraft>(aircraftType, spawnTransform, this, this);
	aircraft->SetGenericTeamId(GetGenericTeamId());
	UGameplayStatics::FinishSpawningActor(aircraft, spawnTransform);

	GetWorld()->GetTimerManager().SetTimer(
		launchCooldownHandle, 
		this, 
		&AAirbase::OnLauchStateCooldownExpired,
		launchCooldownTime,
		false, 
		launchCooldownTime
	);

	AAircraftController* controller = aircraft->GetController<AAircraftController>();
	if (ensure(controller))
	{
		controller->SetHomeBase(this);
	}

	return aircraft;
}

int AAirbase::GetNumberOfFighters() const
{
	return fightersNumber;
}

int AAirbase::GetNumberOfBombers() const
{
	return bombersNumber;
}

void AAirbase::SetNumberOfFighters(int number)
{
	fightersNumber = number;
}

void AAirbase::SetNumberOfBombers(int number)
{
	bombersNumber = number;
}

FGenericTeamId AAirbase::GetGenericTeamId() const
{
	return teamId;
}

void AAirbase::SetGenericTeamId(const FGenericTeamId& team)
{
	teamId = team;
}

bool AAirbase::IsAlive() const
{
	return bIsAlive;
}

UFogOfWarComponent* AAirbase::GetFogOfWarComponent() const
{
	check(fogOfWarComponent);
	return fogOfWarComponent;
}

// Called every frame
void AAirbase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


