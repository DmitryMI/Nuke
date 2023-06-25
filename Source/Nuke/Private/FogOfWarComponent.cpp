// Fill out your copyright notice in the Description page of Project Settings.


#include "FogOfWarComponent.h"
#include "DrawDebugHelpers.h"

void UFogOfWarComponent::UpdateVisibilityInfos()
{
	double time = GetWorld()->GetTimeSeconds();
	for (int team = 0; team < FGenericTeamId::NoTeam; team++)
	{
		FFogOfWarInfo& info = visibilityArray[team];

		FGenericTeamId ownerTeam = GetOwnerTeam();
		ETeamAttitude::Type attitude = FGenericTeamId::GetAttitude(ownerTeam, team);
		bool isAlwaysVisible = (attitude != ETeamAttitude::Hostile);
		if (isAlwaysVisible)
		{
			info.bIsVisible = true;
			continue;
		}

		if (!info.bIsVisible)
		{
			continue;
		}

		double witnessingTimeDelta = time - info.WitnessingTimestamp;
		if (witnessingTimeDelta >= visibilityLingerTime)
		{
			info.bIsVisible = false;
		}
	}
}

FGenericTeamId UFogOfWarComponent::GetOwnerTeam() const
{
	IGenericTeamAgentInterface* teamAgent = GetOwner<IGenericTeamAgentInterface>();
	if (teamAgent)
	{
		return teamAgent->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

// Sets default values for this component's properties
UFogOfWarComponent::UFogOfWarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFogOfWarComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		visibilityUpdateHandle,
		this,
		&UFogOfWarComponent::UpdateVisibilityInfos,
		visibilityUpdatePeriod,
		true,
		0
	);

	FFogOfWarInfo infoDefault;
	visibilityArray.Init(infoDefault, FGenericTeamId::NoTeam);

	APlaytimePlayerController* playerController = GetWorld()->GetFirstPlayerController<APlaytimePlayerController>();
	check(playerController);
	localPlayerState = playerController->GetPlayerState<APlaytimePlayerState>();
}

// Called every frame
void UFogOfWarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FGenericTeamId localPlayerTeam = localPlayerState->GetGenericTeamId();
	if (localPlayerTeam != FGenericTeamId::NoTeam)
	{
		bool visibleForLocalPlayer = IsVisibleForTeam(localPlayerTeam);
		GetOwner()->SetActorHiddenInGame(!visibleForLocalPlayer);
	}
}

bool UFogOfWarComponent::IsVisibleForTeam(const FGenericTeamId& team)
{
	if (team == FGenericTeamId::NoTeam)
	{
		// Neutrals always see everyone
		return true;
	}

	FFogOfWarInfo& info = visibilityArray[team];
	return info.bIsVisible;
}

void UFogOfWarComponent::WitnessUnconditional(const FGenericTeamId& team)
{
	FFogOfWarInfo& info = visibilityArray[team];
	info.bIsVisible = true;
	info.WitnessingTimestamp = GetWorld()->GetTimeSeconds();
	info.WitnessingFrameNumber = GFrameCounter;
}

void UFogOfWarComponent::WitnessIfHasLineOfSight(AActor* sightSourceActor, float maxDistance)
{
	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(sightSourceActor);
	if (!teamAgent)
	{
		return;
	}

	FGenericTeamId team = teamAgent->GetGenericTeamId();

	FFogOfWarInfo& info = visibilityArray[team];
	if (info.WitnessingFrameNumber == GFrameCounter)
	{
		return;
	}

	FVector lineStart = sightSourceActor->GetActorLocation();
	FVector lineEnd = GetOwner()->GetActorLocation();

	float distanceSquare = (lineEnd - lineStart).SizeSquared();
	if (distanceSquare > FMath::Square(maxDistance))
	{
		return;
	}

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActor(sightSourceActor);
	FHitResult hitResult;
	GetWorld()->LineTraceSingleByProfile(hitResult, lineStart, lineEnd, "RadarLineOfSight", queryParams);
	if (hitResult.bBlockingHit)
	{
		return;
	}

	info.bIsVisible = true;
	info.WitnessingTimestamp = GetWorld()->GetTimeSeconds();
	info.WitnessingFrameNumber = GFrameCounter;


#if WITH_EDITOR
	DrawDebugLine(GetWorld(), lineStart, lineEnd, FColor::Cyan);
#endif
}

