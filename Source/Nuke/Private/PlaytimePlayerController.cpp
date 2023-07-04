// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimePlayerController.h"
#include "StrategicControlPawn.h"
#include "City.h"
#include "Attackable.h"

void APlaytimePlayerController::UpdateActorUnderCursor()
{
	FVector worldLocation;
	FVector worldDirection;
	bool deprojectOk = DeprojectMousePositionToWorld(worldLocation, worldDirection);
	if (!deprojectOk)
	{
		actorUnderCursor = false;
	}
}

void APlaytimePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void APlaytimePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APlaytimePlayerController::FindPawnsAlongLine(const FVector& lineStart, const FVector& lineDirection, float lineWidth, TArray<AActor*>& outActors)
{
	TArray<FHitResult> hitResults;

	FVector lineEnd;
	if (AStrategicControlPawn* pawn = GetPawn<AStrategicControlPawn>())
	{
		FVector vecToGround = pawn->GetSightLineWithHorizonIntersection();
		lineEnd = lineDirection + lineDirection * vecToGround.Size() * 1.25f;
	}
	else
	{
		lineEnd = lineStart + lineDirection * 100000;
	}
	// FVector lineEnd = lineStart + lineDirection * 
	FCollisionQueryParams params;
	FCollisionShape sphereShape;
	sphereShape.Sphere.Radius = lineWidth;
	bool hasBlockingHits = GetWorld()->SweepMultiByProfile(hitResults, lineStart, lineEnd, FQuat::Identity, "Pawn", sphereShape, params);

	for (FHitResult hit : hitResults)
	{
		AActor* actor = hit.GetActor();
		if (ACity* city = Cast<ACity>(actor))
		{
			outActors.AddUnique(city);
		}
		else if(IAttackable* attackable = Cast<IAttackable>(actor))
		{
			outActors.AddUnique(actor);
		}
	}

	return outActors.Num() > 0;
}
