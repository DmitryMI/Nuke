// Fill out your copyright notice in the Description page of Project Settings.


#include "StrategicControlPawn.h"
#include "UnitControllerInterface.h"
#include "PlaytimePlayerController.h"
#include "PlaytimePlayerState.h"
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

FVector AStrategicControlPawn::GetSightLineWithHorizonIntersection() const
{
	FVector lineStart = GetActorLocation();
	FVector lineEnd = GetActorForwardVector() * 10000000;
	return FMath::LinePlaneIntersection(lineStart, lineEnd, FVector::ZeroVector, FVector::UpVector);
}

void AStrategicControlPawn::SetGenericTeamId(const FGenericTeamId& teamId)
{
	IGenericTeamAgentInterface* controllerTeamAgent = GetController<IGenericTeamAgentInterface>();
	check(controllerTeamAgent);
	controllerTeamAgent->SetGenericTeamId(teamId);
}

FGenericTeamId AStrategicControlPawn::GetGenericTeamId() const
{
	IGenericTeamAgentInterface* controllerTeamAgent = GetController<IGenericTeamAgentInterface>();
	if (!controllerTeamAgent)
	{
		return FGenericTeamId::NoTeam;
	}
	return controllerTeamAgent->GetGenericTeamId();
}

bool AStrategicControlPawn::CanCommandUnit(AActor* unit) const
{
	FGenericTeamId controllerTeam = GetGenericTeamId();
	if (controllerTeam == FGenericTeamId::NoTeam)
	{
		return false;
	}

	IGenericTeamAgentInterface* unitTeamAgent = Cast<IGenericTeamAgentInterface>(unit);
	if (!unitTeamAgent)
	{
		return false;
	}

	FGenericTeamId unitTeam = unitTeamAgent->GetGenericTeamId();
	return unitTeam == controllerTeam;
}

void AStrategicControlPawn::SubmitGenericPointOrder_Implementation(AActor* unitActor, const FVector& location, bool bQueue)
{
	if (!unitActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Server received a point order for a non-existing unit from player (%d)"), GetGenericTeamId().GetId());
		return;
	}

	if (!CanCommandUnit(unitActor))
	{
		UE_LOG(LogTemp, Error, TEXT("Server received point order for unit %s from player (%d), but this unit is not under player's control"),
			*unitActor->GetName(),
			GetGenericTeamId()
		);
		return;
	}

	APawn* pawn = Cast<APawn>(unitActor);
	check(pawn);

	IUnitControllerInterface* unitController = pawn->GetController<IUnitControllerInterface>();
	if (unitController)
	{
		UE_LOG(LogTemp, Display, TEXT("Server received point order: %s -> %s from player %d"),
			*unitActor->GetName(), *location.ToString(), GetGenericTeamId().GetId()
		);
		unitController->IssueGenericPointOrder(location, bQueue);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Server received point order for unit %s from player (%d), but this unit does not have an attached controller!"),
			*unitActor->GetName(),
			GetGenericTeamId().GetId()
		);
	}
}

void AStrategicControlPawn::SubmitGenericActorOrder_Implementation(AActor* unitActor, AActor* targetOfOrderActor, bool bQueue)
{
	if (!unitActor)
	{
		return;
	}

	if (!targetOfOrderActor)
	{
		return;
	}

	if (!CanCommandUnit(unitActor))
	{
		return;
	}

	APawn* pawn = Cast<APawn>(unitActor);

	IUnitControllerInterface* unitController = pawn->GetController<IUnitControllerInterface>();
	if (unitController)
	{
		unitController->IssueGenericActorOrder(targetOfOrderActor, bQueue);
	}
}

