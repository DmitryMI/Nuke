// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlaytimeHUD.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimeHUD : public AHUD
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	FLinearColor selectionBoxColor = FColor::FromHex("#32CD32");

	UPROPERTY(EditDefaultsOnly)
	float selectionBoxLineWidth = 2.0;

	bool bSelectionBoxActive = false;
	FVector2D selectionBoxStart;
	FVector2D selectionBoxEnd;

protected:
	virtual void DrawHUD() override;

public:
	void StartDrawSelectionBox(FVector2D startingPoint);
	void UpdateSelectionBox(FVector2D endingPoint);
	void EndDrawSelectionBox();
};
