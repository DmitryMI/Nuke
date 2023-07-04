// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimePlayerController.h"
#include "StrategicControlPawn.h"
#include "City.h"
#include "GUIProxyWidget.h"
#include "GUIProxyComponent.h"
#include "Attackable.h"
#include "PlaytimeHUD.h"


void APlaytimePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void APlaytimePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlaytimeHUD* hud = GetHUD<APlaytimeHUD>();
	if (selectPressed && hud)
	{
		double x, y;
		if (GetMousePosition(x, y))
		{
			hud->UpdateSelectionBox(FVector2D(x, y));
		}
	}
}

void APlaytimePlayerController::OnSelectPressed()
{
	double x, y;
	selectPressed = GetMousePosition(x, y);
	selectPressedMousePosition.X = x;
	selectPressedMousePosition.Y = y;

	if (APlaytimeHUD* hud = GetHUD<APlaytimeHUD>())
	{
		hud->StartDrawSelectionBox(selectPressedMousePosition);
	}
}

void APlaytimePlayerController::OnSelectReleased()
{
	if (!selectPressed)
	{
		return;
	}
	selectPressed = false;

	double x, y;
	bool mouseOk = GetMousePosition(x, y);
	if (!mouseOk)
	{
		return;
	}

	APlaytimeHUD* hud = GetHUD<APlaytimeHUD>();
	if (hud)
	{
		hud->EndDrawSelectionBox();
	}

	FVector2D mousePos(x, y);
	double mouseDeltaSquared = (mousePos - selectPressedMousePosition).SizeSquared();
	double minDeltaSquared = multiselectionMouseDeltaMinimum * multiselectionMouseDeltaMinimum;
	if (mouseDeltaSquared < minDeltaSquared)
	{
		// Select single actor under cursor
	}
	else
	{
		// Select multiple actors in selection box
	}
}

void APlaytimePlayerController::OnMouseRightPressed()
{
}

void APlaytimePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	InputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &APlaytimePlayerController::OnSelectPressed);
	InputComponent->BindAction("Select", EInputEvent::IE_Released, this, &APlaytimePlayerController::OnSelectReleased);
}

APlaytimePlayerController::APlaytimePlayerController() : Super()
{
	bShowMouseCursor = true;
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

