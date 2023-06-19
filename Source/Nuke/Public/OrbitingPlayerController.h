// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OrbitingPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API AOrbitingPlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
	float rotationSensitivity = 90.0f;

protected:
	virtual void SetupInputComponent() override;

	void OnMouseX(float value);
	void OnMouseY(float value);
};
