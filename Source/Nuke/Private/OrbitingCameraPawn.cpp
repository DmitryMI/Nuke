// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitingCameraPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Attackable.h"

// Sets default values
AOrbitingCameraPawn::AOrbitingCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrbitingCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOrbitingCameraPawn::SwitchOrbitingTarget()
{
	TArray<AActor*> units;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UAttackable::StaticClass(), units);

	if (orbitingIndex > units.Num())
	{
		orbitingIndex = 0;
	}

	AActor* target = nullptr;
	int counter = 0;
	for (int i = 0; i < orbitingIndex && counter < orbitingIndex; i++)
	{
		IAttackable* attackable = Cast<IAttackable>(units[i]);
		if (!attackable->IsAlive())
		{
			continue;
		}
		target = units[i];
		counter++;
	}

	AttachToActor(target, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	FRotator rotation = FRotator(-30, 30, 0);
	SetActorRotation(rotation);

	orbitingIndex++;
}

void AOrbitingCameraPawn::OnMouseX(float value)
{
	FRotator rotation = GetActorRotation();
	rotation.Yaw += rotationSensitivity * value * GetWorld()->DeltaTimeSeconds;
	SetActorRotation(rotation);
}

void AOrbitingCameraPawn::OnMouseY(float value)
{
	FRotator rotation = GetActorRotation();
	rotation.Pitch += rotationSensitivity * value * GetWorld()->DeltaTimeSeconds;
	SetActorRotation(rotation);
}

// Called every frame
void AOrbitingCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AOrbitingCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MouseX", this, &AOrbitingCameraPawn::OnMouseX);
	InputComponent->BindAxis("MouseY", this, &AOrbitingCameraPawn::OnMouseY);
	InputComponent->BindAction("SwitchOrbitingTarget", EInputEvent::IE_Released, this, &AOrbitingCameraPawn::SwitchOrbitingTarget);
}

