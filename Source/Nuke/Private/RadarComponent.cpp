// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarComponent.h"
#include "Attackable.h"

// Sets default values for this component's properties
URadarComponent::URadarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URadarComponent::BeginPlay()
{
	BindPrimitiveComponentColliders();

	Super::BeginPlay();
}

void URadarComponent::BindPrimitiveComponentColliders()
{
	TArray<USceneComponent*> sceneComponents;
	GetChildrenComponents(false, sceneComponents);

	for (USceneComponent* child : sceneComponents)
	{
		if (UPrimitiveComponent* primitive = Cast<UPrimitiveComponent>(child))
		{
			TArray<AActor*> initialOverlaps;
			primitive->GetOverlappingActors(initialOverlaps);
			for (AActor* actor : initialOverlaps)
			{
				UE_LOG(LogTemp, Display, TEXT("%s: initially overlaps %s"), *GetOwner()->GetName(), *actor->GetName());
				TArray<FOverlapInfo> overlapInfos;
				primitive->GetOverlapsWithActor(actor, overlapInfos);
				for (const FOverlapInfo& overlapInfo : overlapInfos)
				{
					UPrimitiveComponent* otherComponent = overlapInfo.OverlapInfo.GetComponent();
					ProcessInitialOverlap(primitive, actor, otherComponent);
				}
			}

			primitive->OnComponentBeginOverlap.AddDynamic(this, &URadarComponent::OnOverlapBegin);
			primitive->OnComponentEndOverlap.AddDynamic(this, &URadarComponent::OnOverlapEnd);
		}
	}
}

void URadarComponent::OnActorEnteredRadarRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	IAttackable* attackable = Cast<IAttackable>(OtherActor);
	if (!attackable)
	{
		return;
	}

	IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (!teamAgent)
	{
		return;
	}

	if (OtherComp->GetCollisionProfileName() != "Pawn")
	{
		return;
	}

	FGenericTeamId otherTeam = teamAgent->GetGenericTeamId();
	if (FGenericTeamId::GetAttitude(GetGenericTeamId(), otherTeam) != ETeamAttitude::Hostile)
	{
		return;
	}

	if (attackable->IsAlive())
	{
		UE_LOG(LogTemp, Display, TEXT("%s: %s is now in radar range"), *GetOwner()->GetName(), *OtherActor->GetName());
		check(!threatsInRadarRange.Contains(OtherActor));
		threatsInRadarRange.Add(OtherActor);
	}
}

void URadarComponent::ProcessInitialOverlap(UPrimitiveComponent* OverlappedComponent, AActor* actor, UPrimitiveComponent* otherComponent)
{
	OnActorEnteredRadarRange(OverlappedComponent, actor, otherComponent);
}

void URadarComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnActorEnteredRadarRange(OverlappedComponent, OtherActor, OtherComp);
}

void URadarComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	IAttackable* attackable = Cast<IAttackable>(OtherActor);
	if (!attackable)
	{
		return;
	}

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

	if (OtherComp->GetCollisionProfileName() != "Pawn")
	{
		return;
	}

	int index = threatsInRadarRange.IndexOfByKey(OtherActor);

	if (index != INDEX_NONE)
	{
		UE_LOG(LogTemp, Display, TEXT("%s: %s left radar range"), *GetOwner()->GetName(), *OtherActor->GetName());
		threatsInRadarRange.RemoveAt(index);
	}
}

bool URadarComponent::EvaluateTrackingConditions(AActor* actor) const
{
	IAttackable* attackable = Cast<IAttackable>(actor);
	check(attackable);

	if (!attackable->IsAlive())
	{
		return false;
	}

	if (detectableMobilityType != EMobilityEnvironmentType::MET_None)
	{
		if (attackable->GetMobilityEnvironmentType() != detectableMobilityType)
		{
			return false;
		}
	}

	FVector radarLocation = GetOwner()->GetActorLocation() + GetRelativeLocation();
	FVector actorLocation = actor->GetActorLocation();

	double distanceSq = (radarLocation - actorLocation).SizeSquared();
	double detectionRangeFactor = (1.0 - attackable->GetStealthFactor());
	check(0 <= detectionRangeFactor && detectionRangeFactor <= 1);
	
	// If detectionRangeFactor is 1.0, we don't need to recalculate the distance
	if (detectionRangeFactor < 1.0)
	{
		double detectionRange = GetRadarRange() * detectionRangeFactor;
		if (distanceSq > FMath::Square(detectionRange))
		{
			return false;
		}
	}

	TArray<FHitResult> hitResults;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActor(actor);
	GetWorld()->LineTraceMultiByProfile(hitResults, radarLocation, actorLocation, "RadarLineOfSight", queryParams);
	if (hitResults.Num() != 0)
	{
		return false;
	}

	return true;
}


// Called every frame
void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<AActor*> URadarComponent::GetTrackedThreatsArray() const
{
	TArray<AActor*> trackedEnemies;
	GetTrackedThreats(trackedEnemies);
	return trackedEnemies;
}

bool URadarComponent::GetTrackedThreats(TArray<AActor*>& outThreats) const
{
	for (AActor* actor : threatsInRadarRange)
	{
		if (!EvaluateTrackingConditions(actor))
		{
			continue;
		}

		outThreats.Add(actor);
	}
	return outThreats.Num() > 0;
}

void URadarComponent::SetGenericTeamId(const FGenericTeamId& team)
{
	IGenericTeamAgentInterface* ownerTeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner());
	check(ownerTeamAgent);
	ownerTeamAgent->SetGenericTeamId(team);
}

FGenericTeamId URadarComponent::GetGenericTeamId() const
{
	IGenericTeamAgentInterface* ownerTeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner());
	check(ownerTeamAgent);
	return ownerTeamAgent->GetGenericTeamId();
}

bool URadarComponent::IsActorTrackedByRadar(AActor* actor) const
{
	if (!threatsInRadarRange.Contains(actor))
	{
		return false;
	}

	return EvaluateTrackingConditions(actor);
}

EMobilityEnvironmentType URadarComponent::GetDetectableMobilityType() const
{
	return detectableMobilityType;
}

void URadarComponent::SetDetectableMobilityType(EMobilityEnvironmentType mobilityType)
{
	detectableMobilityType = mobilityType;
}

void URadarComponent::SetRadarRange(float range)
{
	radarRange = range;
}

float URadarComponent::GetRadarRange() const
{
	return radarRange;
}


