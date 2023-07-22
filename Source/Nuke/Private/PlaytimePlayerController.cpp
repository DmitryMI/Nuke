// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytimePlayerController.h"
#include "StrategicControlPawn.h"
#include "City.h"
#include "GUIProxyWidget.h"
#include "GUIProxyComponent.h"
#include "Attackable.h"
#include "NiagaraComponent.h"
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
	double minDeltaSquared = mouseMovementThreshold * mouseMovementThreshold;
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

void APlaytimePlayerController::OnCommandPressed(bool bQueue)
{
	double x, y;
	commandPressed = GetMousePosition(x, y);
	commandPressedMousePosition.X = x;
	commandPressedMousePosition.Y = y;
}

void APlaytimePlayerController::OnCommandReleased(bool bQueue)
{
	if (!commandPressed)
	{
		return;
	}
	commandPressed = false;

	double x, y;
	bool mouseOk = GetMousePosition(x, y);
	if (!mouseOk)
	{
		return;
	}

	FVector2D mousePos(x, y);
	double mouseDeltaSquared = (mousePos - commandPressedMousePosition).SizeSquared();
	double minDeltaSquared = mouseMovementThreshold * mouseMovementThreshold;
	if (mouseDeltaSquared < minDeltaSquared)
	{
		FVector landscapeLocation;
		bool deprojOk = DeprojectScreenPositionToLandscape(mousePos, landscapeLocation);
		if (deprojOk)
		{
			SpawnMoveToEffect(mousePos, landscapeLocation, moveToEffectColor);

			SubmitGenericPointOrders(selectedActors, landscapeLocation, bQueue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to deproject screen location %s on Landscape"), *mousePos.ToString());
		}
	}
}

void APlaytimePlayerController::OnCommandPressedImmediate()
{
	OnCommandPressed(false);
}

void APlaytimePlayerController::OnCommandReleasedImmediate()
{
	OnCommandReleased(false);
}

void APlaytimePlayerController::OnCommandPressedQueue()
{
	OnCommandPressed(true);
}

void APlaytimePlayerController::OnCommandReleasedQueue()
{
	OnCommandReleased(true);
}

void APlaytimePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	InputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &APlaytimePlayerController::OnSelectPressed);
	InputComponent->BindAction("Select", EInputEvent::IE_Released, this, &APlaytimePlayerController::OnSelectReleased);

	InputComponent->BindAction("Command", EInputEvent::IE_Pressed, this, &APlaytimePlayerController::OnCommandPressedImmediate);
	InputComponent->BindAction("Command", EInputEvent::IE_Released, this, &APlaytimePlayerController::OnCommandReleasedImmediate);

	InputComponent->BindAction("CommandQueue", EInputEvent::IE_Pressed, this, &APlaytimePlayerController::OnCommandPressedQueue);
	InputComponent->BindAction("CommandQueue", EInputEvent::IE_Released, this, &APlaytimePlayerController::OnCommandReleasedQueue);
}

void APlaytimePlayerController::SubmitGenericPointOrders(const TArray<AActor*> unitGroup, const FVector& targetLocation, bool bQueue)
{
	if (unitGroup.Num() == 0)
	{
		return;
	}

	AStrategicControlPawn* pawn = GetPawn<AStrategicControlPawn>();
	if (!pawn)
	{
		return;
	}

	for (AActor* actor : unitGroup)
	{
		if (!pawn->CanCommandUnit(actor))
		{
			UE_LOG(LogTemp, Error, TEXT("Local player (team: %d): cannot command unit %s"), GetGenericTeamId(), *targetLocation.ToString(), *actor->GetName());
			continue;
		}
		UE_LOG(LogTemp, Display, TEXT("Local player (team: %d): submitting point order %s for %s (queue: %d)"), GetGenericTeamId(), *targetLocation.ToString(), *actor->GetName(), bQueue);
		pawn->SubmitGenericPointOrder(actor, targetLocation, bQueue);
	}
}

void APlaytimePlayerController::SpawnMoveToEffect(const FVector2D& screenLocation, const FVector& worldLocation, const FLinearColor& color)
{
	if (!moveToEffect)
	{
		return;
	}

	FVector centerPointWorld;
	FVector centerPointWorldDir;
	bool deprojOk = DeprojectScreenPositionToWorld(screenLocation.X, screenLocation.Y, centerPointWorld, centerPointWorldDir);
	
	FVector2D leftPoint = screenLocation - FVector2D(moveToEffectScreenSize / 2, 0);
	FVector2D rightPoint = screenLocation + FVector2D(moveToEffectScreenSize / 2, 0);
	FVector leftPointWorld;
	FVector leftPointWorldDir;
	FVector rightPointWorld;
	FVector rightPointWorldDir;
	deprojOk &= DeprojectScreenPositionToWorld(leftPoint.X, leftPoint.Y, leftPointWorld, leftPointWorldDir);
	deprojOk &= DeprojectScreenPositionToWorld(rightPoint.X, rightPoint.Y, rightPointWorld, rightPointWorldDir);
	if (!deprojOk)
	{
		return;
	}

	double distanceFromCamera = (worldLocation - centerPointWorld).Size();

	leftPointWorld += leftPointWorldDir * distanceFromCamera;
	rightPointWorld += rightPointWorldDir * distanceFromCamera;

	double worldSize = (leftPointWorld - rightPointWorld).Size();
	double worldScale = worldSize / 100.0f;

	UNiagaraComponent* effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		moveToEffect,
		worldLocation,
		FRotator::ZeroRotator,
		FVector(worldScale, worldScale, worldScale),
		true,
		true,
		ENCPoolMethod::AutoRelease,
		false
	);

	if (!effect)
	{
		return;
	}

	effect->SetVariableLinearColor("Color", color);
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

bool APlaytimePlayerController::DeprojectScreenPositionToLandscape(const FVector2D& screenLocation, FVector& landscapeLocation) const
{
	FVector deprojectedWorldLocation;
	FVector deprojectedWorldDirection;
	bool deprojectOk = DeprojectScreenPositionToWorld(screenLocation.X, screenLocation.Y, deprojectedWorldLocation, deprojectedWorldDirection);
	if (!deprojectOk)
	{
		return false;
	}

	FHitResult hitResult;
	FVector traceEnd = deprojectedWorldLocation + deprojectedWorldDirection * 1000000.0f;
	bool traceHasHit = GetWorld()->LineTraceSingleByChannel(hitResult, deprojectedWorldLocation, traceEnd, CHANNEL_LANDSCAPE_TRACE);
	if (!traceHasHit)
	{
		return false;
	}

	landscapeLocation = hitResult.ImpactPoint;

	return true;
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

void APlaytimePlayerController::SetGenericTeamId(const FGenericTeamId& teamId)
{
	APlaytimePlayerState* playerState = GetPlayerState<APlaytimePlayerState>();
	check(playerState);
	playerState->SetGenericTeamId(teamId);
}

FGenericTeamId APlaytimePlayerController::GetGenericTeamId() const
{
	APlaytimePlayerState* playerState = GetPlayerState<APlaytimePlayerState>();
	if (!playerState)
	{
		return FGenericTeamId::NoTeam;
	}
	
	return playerState->GetGenericTeamId();
}

