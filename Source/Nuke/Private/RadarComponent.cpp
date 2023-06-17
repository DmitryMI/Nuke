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

void URadarComponent::OnActorEnteredRadarRage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp)
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
	OnActorEnteredRadarRage(OverlappedComponent, actor, otherComponent);
}

void URadarComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnActorEnteredRadarRage(OverlappedComponent, OtherActor, OtherComp);
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

bool URadarComponent::IsTracked(AActor* actor) const
{
	IAttackable* attackable = Cast<IAttackable>(actor);
	check(attackable);

	if (!attackable->IsAlive())
	{
		return false;
	}

	TArray<FHitResult> hitResults;
	FVector traceStart = GetOwner()->GetActorLocation() + GetRelativeLocation();
	FVector traceEnd = actor->GetActorLocation();
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActor(actor);
	GetWorld()->LineTraceMultiByProfile(hitResults, traceStart, traceEnd, "RadarLineOfSight", queryParams);
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

TArray<AActor*> URadarComponent::GetTrackedThreats() const
{
	TArray<AActor*> trackedEnemies;
	for (AActor* actor : threatsInRadarRange)
	{
		if (!IsTracked(actor))
		{
			continue;
		}

		trackedEnemies.Add(actor);
	}
	return trackedEnemies;
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

