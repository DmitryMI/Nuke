// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OrbitingCameraPawn.generated.h"

UCLASS()
class NUKE_API AOrbitingCameraPawn : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float rotationSensitivity = 90.0f;

	UPROPERTY(EditAnywhere)
	bool bAllowEnemyUnits = true;

	int orbitingIndex = 0;

public:
	// Sets default values for this pawn's properties
	AOrbitingCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SwitchOrbitingTarget();

	void OnMouseX(float value);
	void OnMouseY(float value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
