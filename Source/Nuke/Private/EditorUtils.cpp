// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtils.h"
#include "Kismet/GameplayStatics.h"
#include "WaterBodyComponent.h"
#include "WaterBodyIslandActor.h"
#include "NavMesh/RecastNavMesh.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "NavModifierComponent.h"

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

void UEditorUtils::SpawnShallowWaterNavModifiers(UObject* worldContext, TSubclassOf<AActor> modifierVolumeClass, TSubclassOf<UNavArea> shallowWaterNavAreaOverride)
{
	const float shallowWaterWidth = 5000;
	const int modifiersBetweenSplinePoints = 4;

	TArray<AActor*> existingModifierActors;
	UGameplayStatics::GetAllActorsOfClass(worldContext, modifierVolumeClass, existingModifierActors);

	UWorld* world = GEngine->GetWorldFromContextObject(worldContext, EGetWorldErrorMode::LogAndReturnNull);

	TArray<AActor*> islandActors;
	UGameplayStatics::GetAllActorsOfClass(worldContext, AWaterBodyIsland::StaticClass(), islandActors);

	int modifierCounter = 0;

	for (AActor* islandActor : islandActors)
	{
		USplineComponent* islandSplineComponent = islandActor->GetComponentByClass<USplineComponent>();
		if (!islandSplineComponent)
		{
			continue;
		}

		int splinePointsNum = islandSplineComponent->GetNumberOfSplinePoints();

		for (int i = 0; i < splinePointsNum; i++)
		{
			int segmentStartIndex = i;
			int segmentEndIndex = i + 1;

			float segmentStartDistance = islandSplineComponent->GetDistanceAlongSplineAtSplinePoint(segmentStartIndex);
			float segmentEndDistance = islandSplineComponent->GetDistanceAlongSplineAtSplinePoint(segmentEndIndex);

			float distanceBetweenSubsegments = (segmentEndDistance - segmentStartDistance) / modifiersBetweenSplinePoints;
			for (int j = 0; j < modifiersBetweenSplinePoints; j++)
			{
				float modifierStartDistance = segmentStartDistance + j * distanceBetweenSubsegments;
				float modifierEndDistance = segmentStartDistance + (j + 1) * distanceBetweenSubsegments;
				FVector startLocation = islandSplineComponent->GetWorldLocationAtDistanceAlongSpline(modifierStartDistance);
				FVector endLocation = islandSplineComponent->GetWorldLocationAtDistanceAlongSpline(modifierEndDistance);

				FVector startTangent = islandSplineComponent->GetTangentAtDistanceAlongSpline(modifierStartDistance, ESplineCoordinateSpace::World);
				FVector endTangent = islandSplineComponent->GetTangentAtDistanceAlongSpline(modifierEndDistance, ESplineCoordinateSpace::World);
				startTangent.Normalize();
				endTangent.Normalize();
				
				FVector startNormal = startTangent.RotateAngleAxis(-90, FVector::UpVector);
				FVector endNormal = endTangent.RotateAngleAxis(-90, FVector::UpVector);
				
				FVector startOffset = startLocation + startNormal * shallowWaterWidth;
				FVector endOffset = endLocation + endNormal * shallowWaterWidth;

				DrawDebugLine(world, startLocation, startOffset, FColor::Green, false, 5.0f, 0);
				DrawDebugLine(world, endLocation, endOffset, FColor::Cyan, false, 5.0f, 0);

				DrawDebugLine(world, startLocation, endLocation, FColor::Red, false, 5.0f, 0, 10);
				DrawDebugLine(world, startOffset, endOffset, FColor::Blue, false, 5.0f, 0, 10);

				FVector averageNormal = ((startNormal + endNormal) / 2).GetUnsafeNormal();
				FVector modifierBoxCenter = (startOffset + endOffset) / 2 - averageNormal * shallowWaterWidth / 2;

				FRotator modifierBoxRotation = averageNormal.Rotation();
				FQuat modifierBoxQuat = modifierBoxRotation.Quaternion();
				float modifierLength = (startOffset - endOffset).Size();

				FVector modifierBoxExtent = FVector(shallowWaterWidth / 2, modifierLength / 2, 500);
				DrawDebugBox(
					world, 
					modifierBoxCenter,
					modifierBoxExtent,
					modifierBoxQuat,
					FColor::Orange, 
					false, 
					5.0f
				);

				if (!modifierVolumeClass)
				{
					continue;
				}

				FActorSpawnParameters spawnParams;
				spawnParams.bNoFail = true;

				AActor* modifierActor;
				if (modifierCounter >= existingModifierActors.Num())
				{
					modifierActor = world->SpawnActor<AActor>(modifierVolumeClass, modifierBoxCenter, modifierBoxRotation, spawnParams);
				}
				else
				{
					modifierActor = existingModifierActors[modifierCounter];
					modifierActor->SetActorLocationAndRotation(modifierBoxCenter, modifierBoxRotation);
				}

				modifierActor->SetActorScale3D(FVector(1, 1, 1));

				UBoxComponent* boxComponent = modifierActor->GetComponentByClass<UBoxComponent>();
				if (!boxComponent)
				{
					UE_LOG(LogTemp, Error, TEXT("UBoxComponent not found on actor %s!"), *modifierActor->GetName());
					return;
				}
				boxComponent->SetBoxExtent(modifierBoxExtent);

				if (shallowWaterNavAreaOverride)
				{
					UNavModifierComponent* navModifierComponent = modifierActor->GetComponentByClass<UNavModifierComponent>();
					navModifierComponent->SetAreaClass(shallowWaterNavAreaOverride);
				}

				modifierActor->SetFolderPath("Navigation/ShallowWaterModifiers");

				modifierCounter++;
			}
		}
	}
}
