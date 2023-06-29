// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtils.h"
#include "Kismet/GameplayStatics.h"
#include "WaterBodyComponent.h"
#include "WaterBodyIslandActor.h"
#include "WaterBodyOceanActor.h"
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

void UEditorUtils::SpawnNavModifiersAroundSpline(UObject* worldContext, TSubclassOf<AActor> modifierVolumeClass, USplineComponent* splineComponent, int subsegmentsNum, float width, TArray<AActor*> modifierPool, int& modifierPoolIndex, TSubclassOf<UNavArea> navAreaOverride)
{
	UWorld* world = GEngine->GetWorldFromContextObject(worldContext, EGetWorldErrorMode::LogAndReturnNull);

	int splinePointsNum = splineComponent->GetNumberOfSplinePoints();

	for (int i = 0; i < splinePointsNum; i++)
	{
		int segmentStartIndex = i;
		int segmentEndIndex = i + 1;

		float segmentStartDistance = splineComponent->GetDistanceAlongSplineAtSplinePoint(segmentStartIndex);
		float segmentEndDistance = splineComponent->GetDistanceAlongSplineAtSplinePoint(segmentEndIndex);

		float distanceBetweenSubsegments = (segmentEndDistance - segmentStartDistance) / subsegmentsNum;
		for (int j = 0; j < subsegmentsNum; j++)
		{
			float modifierStartDistance = segmentStartDistance + j * distanceBetweenSubsegments;
			float modifierEndDistance = segmentStartDistance + (j + 1) * distanceBetweenSubsegments;
			FVector startLocation = splineComponent->GetWorldLocationAtDistanceAlongSpline(modifierStartDistance);
			FVector endLocation = splineComponent->GetWorldLocationAtDistanceAlongSpline(modifierEndDistance);

			FVector startTangent = splineComponent->GetTangentAtDistanceAlongSpline(modifierStartDistance, ESplineCoordinateSpace::World);
			FVector endTangent = splineComponent->GetTangentAtDistanceAlongSpline(modifierEndDistance, ESplineCoordinateSpace::World);
			startTangent.Normalize();
			endTangent.Normalize();

			FVector startNormal = startTangent.RotateAngleAxis(-90, FVector::UpVector);
			FVector endNormal = endTangent.RotateAngleAxis(-90, FVector::UpVector);

			FVector startOffset = startLocation + startNormal * width;
			FVector endOffset = endLocation + endNormal * width;

			DrawDebugLine(world, startLocation, startOffset, FColor::Green, false, 5.0f, 0);
			DrawDebugLine(world, endLocation, endOffset, FColor::Cyan, false, 5.0f, 0);

			DrawDebugLine(world, startLocation, endLocation, FColor::Red, false, 5.0f, 0, 10);
			DrawDebugLine(world, startOffset, endOffset, FColor::Blue, false, 5.0f, 0, 10);

			//FVector averageNormal = ((startNormal + endNormal) / 2).GetUnsafeNormal();
			float averageDistance = (modifierEndDistance + modifierStartDistance) / 2;
			FVector averageTangent = splineComponent->GetTangentAtDistanceAlongSpline(averageDistance, ESplineCoordinateSpace::World);
			averageTangent.Normalize();
			FVector averageNormal = averageTangent.RotateAngleAxis(-90, FVector::UpVector);
			FVector modifierBoxCenter = (startOffset + endOffset) / 2 - averageNormal * width / 2;

			FRotator modifierBoxRotation = averageNormal.Rotation();
			FQuat modifierBoxQuat = modifierBoxRotation.Quaternion();
			float modifierLength = (startOffset - endOffset).Size();

			FVector modifierBoxExtent = FVector(width / 2, modifierLength / 2, 500);
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
			if (modifierPoolIndex >= modifierPool.Num())
			{
				modifierActor = world->SpawnActor<AActor>(modifierVolumeClass, modifierBoxCenter, modifierBoxRotation, spawnParams);
			}
			else
			{
				modifierActor = modifierPool[modifierPoolIndex];
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

			if (navAreaOverride)
			{
				UNavModifierComponent* navModifierComponent = modifierActor->GetComponentByClass<UNavModifierComponent>();
				navModifierComponent->SetAreaClass(navAreaOverride);
			}

			modifierActor->SetFolderPath("Navigation/ShallowWaterModifiers");

			modifierPoolIndex++;
		}
	}
}

void UEditorUtils::SpawnShallowWaterNavModifiers(UObject* worldContext, TSubclassOf<AActor> modifierVolumeClass, TSubclassOf<UNavArea> shallowWaterNavAreaOverride)
{
	const float shallowWaterWidth = 8000;
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

		SpawnNavModifiersAroundSpline(
			worldContext,
			modifierVolumeClass,
			islandSplineComponent,
			modifiersBetweenSplinePoints,
			shallowWaterWidth,
			existingModifierActors,
			modifierCounter,
			shallowWaterNavAreaOverride
		);
	}

	TArray<AActor*> oceanActors;
	UGameplayStatics::GetAllActorsOfClass(worldContext, AWaterBodyOcean::StaticClass(), oceanActors);

	for (AActor* oceanActor : oceanActors)
	{
		USplineComponent* oceanSplineComponent = oceanActor->GetComponentByClass<USplineComponent>();
		if (!oceanSplineComponent)
		{
			continue;
		}

		SpawnNavModifiersAroundSpline(
			worldContext,
			modifierVolumeClass,
			oceanSplineComponent,
			modifiersBetweenSplinePoints,
			shallowWaterWidth,
			existingModifierActors,
			modifierCounter,
			shallowWaterNavAreaOverride
		);
	}

	int modifiersLeft = existingModifierActors.Num() - modifierCounter;
	for (int i = 0; i < modifiersLeft; i++)
	{
		AActor* actor = existingModifierActors.Pop();
		actor->Destroy();
	}
}
