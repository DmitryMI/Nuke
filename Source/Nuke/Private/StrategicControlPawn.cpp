// Fill out your copyright notice in the Description page of Project Settings.


#include "StrategicControlPawn.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AStrategicControlPawn::AStrategicControlPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicateMovement(false);
	bReplicates = true;
}

void AStrategicControlPawn::BeginPlay()
{
	Super::BeginPlay();

	FVector location = GetActorLocation();
	location.Z = cameraOriginZ;
	SetActorLocation(location);
	SetActorRotation(cameraRotation);
	SetZoom(cameraZoom);
}

void AStrategicControlPawn::OnScrollVerticalAxis(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		return;
	}

	float factor = 0.5 * cameraZoom / cameraZoomMinMax.X + 0.5;
	float delta = cameraScrollingSpeed* GetWorld()->DeltaTimeSeconds * value * factor;

	FVector location = GetActorLocation();
	FVector deltaVec = FVector(delta, 0 , 0);
	deltaVec = deltaVec.RotateAngleAxis(cameraRotation.Yaw, FVector::UpVector);
	location += deltaVec;
	SetActorLocation(location);
}

void AStrategicControlPawn::OnScrollHorizontalAxis(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		return;
	}

	float factor = 0.5 * cameraZoom / cameraZoomMinMax.X + 0.5;

	float delta = cameraScrollingSpeed * GetWorld()->DeltaTimeSeconds * value * factor;
	FVector location = GetActorLocation();
	FVector deltaVec = FVector(0, delta, 0);
	deltaVec = deltaVec.RotateAngleAxis(cameraRotation.Yaw, FVector::UpVector);
	location += deltaVec;
	SetActorLocation(location);
}

void AStrategicControlPawn::OnCameraZoomAxis(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		return;
	}

	USpringArmComponent* arm = GetComponentByClass<USpringArmComponent>();
	if (!arm)
	{
		return;
	}
	float factor = 0.5 * cameraZoom / cameraZoomMinMax.X + 0.5;
	float zoom = cameraZoom - value * cameraZoomSpeed * GetWorld()->DeltaTimeSeconds * factor;
	SetZoom(zoom);
}

// Called every frame
void AStrategicControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStrategicControlPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("ScrollHorizontal", this, &AStrategicControlPawn::OnScrollHorizontalAxis);
	PlayerInputComponent->BindAxis("ScrollVertical", this, &AStrategicControlPawn::OnScrollVerticalAxis);
	PlayerInputComponent->BindAxis("Zoom", this, &AStrategicControlPawn::OnCameraZoomAxis);
}

void AStrategicControlPawn::SetZoom(float zoom)
{
	cameraZoom = FMath::Clamp(zoom, cameraZoomMinMax.X, cameraZoomMinMax.Y);

	USpringArmComponent* arm = GetComponentByClass<USpringArmComponent>();
	if (!arm)
	{
		return;
	}

	arm->TargetArmLength = cameraZoom;
}

