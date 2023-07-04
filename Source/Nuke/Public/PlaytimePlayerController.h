// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlaytimePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimePlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	TArray<AActor*> selectedActors;

	AActor* actorUnderCursor;
	
	void UpdateActorUnderCursor();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	bool FindPawnsAlongLine(const FVector& lineStart, const FVector& lineDirection, float lineWidth, TArray<AActor*>& outActors);
};
