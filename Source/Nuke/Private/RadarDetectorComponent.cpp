// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarDetectorComponent.h"
#include "RadarComponent.h"
#include "Attackable.h"
#include "GenericTeamAgentInterface.h"
#include "FogOfWarComponent.h"

// Sets default values for this component's properties
URadarDetectorComponent::URadarDetectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URadarDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

}

FRadarDetectorNotification& URadarDetectorComponent::OnRadarDetectorNotification()
{
	return onRadarDetectorNotification;
}


// Called every frame
void URadarDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URadarDetectorComponent::NotifyDetection(const AActor* sender)
{
	AActor* actor = const_cast<AActor*>(sender);

	if (detectedIrradiators.Contains(sender))
	{
		FRadarDetectionInfo& info = detectedIrradiators[sender];
		info.UpdateTimestamp = GetWorld()->GetTimeSeconds();
		onRadarDetectorNotification.Broadcast(this, actor, info);
	}
	else
	{
		FRadarDetectionInfo info;
		info.Irradiator = actor;
		info.DetectionTimestamp = GetWorld()->GetTimeSeconds();
		info.UpdateTimestamp = GetWorld()->GetTimeSeconds();
		detectedIrradiators.Add(actor, info);
		onRadarDetectorNotification.Broadcast(this, actor, info);
	}

	IAttackable* ownerAttackable = GetOwner<IAttackable>();
	IGenericTeamAgentInterface* ownerTeamAgent = GetOwner<IGenericTeamAgentInterface>();
	UFogOfWarComponent* fowComponent = ownerAttackable->GetFogOfWarComponent();
	if (fowComponent && ownerTeamAgent)
	{
		fowComponent->WitnessUnconditional(ownerTeamAgent->GetGenericTeamId());
	}
}

void URadarDetectorComponent::ClearDetectionData()
{
	detectedIrradiators.Empty();
}

bool URadarDetectorComponent::GetDetectedRadars(TArray<AActor*>& outRadarActors) const
{
	return GetDetectedRadars<AActor>(outRadarActors);
}

