// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtils.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/RecastNavMesh.h"
#include "WaterBodyComponent.h"
#include "NavigationSystem.h"

void UEditorUtils::SetWaterNavArea(UObject* worldContext, TSubclassOf<UNavArea> navAreaClass)
{
	UWorld* world = GEngine->GetWorldFromContextObject(worldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!ensure(world))
	{
		return;
	}

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(worldContext, AActor::StaticClass(), actors);

	for (AActor* actor : actors)
	{
		UWaterBodyComponent* waterBody = actor->GetComponentByClass<UWaterBodyComponent>();
		if (!waterBody)
		{
			continue;
		}

		waterBody->SetNavAreaClass(navAreaClass);
	}
	
}

void UEditorUtils::SetLandNavArea(UObject* worldContext, TSubclassOf<UNavArea> navAreaClass)
{
	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(worldContext);

	for (auto navData : navSystem->NavDataSet)
	{
		if (ARecastNavMesh* recastNavMesh = Cast<ARecastNavMesh>(navData))
		{
			UE_LOG(LogTemp, Display, TEXT("RecastNavMesh found: %s"), *recastNavMesh->GetName());
		}
	}
}

TArray<UClass*> UEditorUtils::GetAllSubclasses(UClass* clazz)
{
	TArray<UClass*> result;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(clazz) && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			result.Add(*It);
		}
	}

	return result;
}

UClass* UEditorUtils::GetClassByDisplayName(TArray<UClass*> clazzes, const FString& displayName)
{
	for (UClass* clazz : clazzes)
	{
		if (clazz->GetName() == displayName)
		{
			return clazz;
		}
	}

	return nullptr;
}