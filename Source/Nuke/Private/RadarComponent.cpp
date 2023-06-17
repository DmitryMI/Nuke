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
	Super::BeginPlay();

	BindPrimitiveComponentColliders();
}

void URadarComponent::BindPrimitiveComponentColliders()
{
	TArray<USceneComponent*> sceneComponents;
	GetChildrenComponents(false, sceneComponents);

	for (USceneComponent* child : sceneComponents)
	{
		if (UPrimitiveComponent* primitive = Cast<UPrimitiveComponent>(child))
		{
			primitive->OnComponentBeginOverlap.AddDynamic(this, &URadarComponent::OnOverlapBegin);
			primitive->OnComponentEndOverlap.AddDynamic(this, &URadarComponent::OnOverlapEnd);
		}
	}
}

void URadarComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		UE_LOG(LogTemp, Display, TEXT("%s now tracks %s"), *GetOwner()->GetName(), *OtherActor->GetName());
		threatsInRadarRange.Add(OtherActor);
	}
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

	if (threatsInRadarRange.Contains(OtherActor))
	{
		UE_LOG(LogTemp, Display, TEXT("%s lost track on %s"), *GetName(), *OtherActor->GetName());
		threatsInRadarRange.Remove(OtherActor);
	}
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
		IAttackable* attackable = Cast<IAttackable>(actor);
		check(attackable);

		if (!attackable->IsAlive())
		{
			continue;
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

