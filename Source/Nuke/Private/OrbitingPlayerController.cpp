// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitingPlayerController.h"

void AOrbitingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	ensure(InputComponent);
	InputComponent->BindAxis("MouseX", this, &AOrbitingPlayerController::OnMouseX);
	InputComponent->BindAxis("MouseY", this, &AOrbitingPlayerController::OnMouseY);
}

void AOrbitingPlayerController::OnMouseX(float value)
{
	APawn* pawn = GetPawn();
	if (!pawn)
	{
		return;
	}

	FRotator rotation = pawn->GetActorRotation();
	rotation.Yaw += rotationSensitivity * value * GetWorld()->DeltaTimeSeconds;
	pawn->SetActorRotation(rotation);
}

void AOrbitingPlayerController::OnMouseY(float value)
{
	APawn* pawn = GetPawn();
	if (!pawn)
	{
		return;
	}

	FRotator rotation = pawn->GetActorRotation();
	rotation.Pitch += rotationSensitivity * value * GetWorld()->DeltaTimeSeconds;
	pawn->SetActorRotation(rotation);
}
