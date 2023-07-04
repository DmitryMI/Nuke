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
		ClearSelection();

		FindUnitsInsideScreenBox(selectPressedMousePosition, mousePos, selectedActors);
		for (AActor* actor : selectedActors)
		{
			UGUIProxyComponent* proxyComponent = actor->GetComponentByClass<UGUIProxyComponent>();
			check(proxyComponent);
			proxyComponent->SetIsSelectedInGui(true);
		}
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

void APlaytimePlayerController::AddToSelection(AActor* actor)
{
	UGUIProxyComponent* proxyComponent = actor->GetComponentByClass<UGUIProxyComponent>();
	if (!proxyComponent)
	{
		return;
	}
	proxyComponent->SetIsSelectedInGui(true);

	selectedActors.AddUnique(actor);
}

void APlaytimePlayerController::ClearSelection()
{
	for (AActor* actor : selectedActors)
	{
		UGUIProxyComponent* proxyComponent = actor->GetComponentByClass<UGUIProxyComponent>();
		if (!proxyComponent)
		{
			continue;
		}
		proxyComponent->SetIsSelectedInGui(false);
	}

	selectedActors.Empty(12);
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

bool APlaytimePlayerController::FindUnitsInsideScreenBox(const FVector2D& boxStart, const FVector2D& boxEnd, TArray<AActor*>& outActors) const
{
	APlaytimePlayerState* playerState = GetPlayerState<APlaytimePlayerState>();
	check(playerState);

	for (AActor* actor : playerState->GetPlayerUnits())
	{
		UGUIProxyComponent* proxyComponent = actor->GetComponentByClass<UGUIProxyComponent>();
		if (!proxyComponent)
		{
			continue;
		}

		FVector2D screenLocation;
		bool widgetVisible = proxyComponent->IsWidgetVisible();
		if (!widgetVisible)
		{
			continue;
		}

		if (!ProjectWorldLocationToScreen(actor->GetActorLocation(), screenLocation))
		{
			continue;
		}

		double minX, maxX, minY, maxY;
		if (boxStart.X < boxEnd.X)
		{
			minX = boxStart.X;
			maxX = boxEnd.X;
		}
		else
		{
			minX = boxEnd.X;
			maxX = boxStart.X;
		}

		if (boxStart.Y < boxEnd.Y)
		{
			minY = boxStart.Y;
			maxY = boxEnd.Y;
		}
		else
		{
			minY = boxEnd.Y;
			maxY = boxStart.Y;
		}

		if (
			minX <= screenLocation.X && screenLocation.X <= maxX &&
			minY <= screenLocation.Y && screenLocation.Y <= maxY
			)
		{
			outActors.Add(actor);
		}
	}

	return outActors.Num() > 0;
}

