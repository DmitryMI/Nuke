// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GUIProxyWidget.h"
#include "PlaytimePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimePlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	double multiselectionMouseDeltaMinimum = 5.0;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> selectedActors;

	bool selectPressed = false;
	FVector2D selectPressedMousePosition;
	

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void OnSelectPressed();
	void OnSelectReleased();

	void OnMouseRightPressed();

protected:
	virtual void SetupInputComponent() override;

public:
	APlaytimePlayerController();

	UFUNCTION(BlueprintCallable)
	bool FindPawnsAlongLine(const FVector& lineStart, const FVector& lineDirection, float lineWidth, TArray<AActor*>& outActors);


};
