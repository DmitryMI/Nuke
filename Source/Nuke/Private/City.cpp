// Fill out your copyright notice in the Description page of Project Settings.


#include "City.h"
#include "PlaytimeUtils.h"
#include "PlaytimePlayerState.h"

#if WITH_EDITOR  
void ACity::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetName() == "radius")
	{
		cityRadiusCollider->SetSphereRadius(radius);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

// Sets default values
ACity::ACity()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root", true);

	cityRadiusCollider = CreateDefaultSubobject<USphereComponent>("RadiusCollider", true);
	cityRadiusCollider->SetCollisionProfileName(TEXT("CityRadius"));
	cityRadiusCollider->SetSphereRadius(radius);
	cityRadiusCollider->SetupAttachment(RootComponent);
	//cityRadiusCollider->RegisterComponent();
}

// Called when the game starts or when spawned
void ACity::BeginPlay()
{
	Super::BeginPlay();
	
	APlaytimePlayerState* playerState = UPlaytimeUtils::GetOwningPlayerState(this);
	if (playerState && GetLocalRole() == ROLE_Authority)
	{
		playerState->GetPlayerCitiesMutable().AddUnique(this);
	}
}

// Called every frame
void ACity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACity::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	teamId = TeamID;
}

FGenericTeamId ACity::GetGenericTeamId() const
{
	return teamId;
}

