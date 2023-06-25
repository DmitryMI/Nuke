// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StrategicControlPawn.generated.h"

UCLASS()
class NUKE_API AStrategicControlPawn : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float cameraScrollingSpeed = 1000.0f;

	UPROPERTY(EditAnywhere)
	float cameraZoomSpeed = 2000.0f;

	UPROPERTY(EditAnywhere)
	FVector2D cameraZoomMinMax = FVector2D(500.0f, 10000.0f);

	UPROPERTY(EditAnywhere)
	float cameraZoom = 5000.0f;

	UPROPERTY(EditAnywhere)
	float cameraOriginZ = 0.0f;

	UPROPERTY(EditAnywhere)
	FRotator cameraRotation = FRotator(-45, -60, 0);

public:
	// Sets default values for this pawn's properties
	AStrategicControlPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnScrollVerticalAxis(float value);
	void OnScrollHorizontalAxis(float value);
	void OnCameraZoomAxis(float value);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetZoom(float zoom);
};
