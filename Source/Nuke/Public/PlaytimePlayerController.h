// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GUIProxyWidget.h"
#include "GenericTeamAgentInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "PlaytimePlayerController.generated.h"

#define CHANNEL_LANDSCAPE_TRACE ECC_GameTraceChannel4

/**
 * 
 */
UCLASS()
class NUKE_API APlaytimePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	double mouseMovementThreshold = 5.0;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> selectedActors;

	bool selectPressed = false;
	FVector2D selectPressedMousePosition;
	
	bool commandPressed = false;
	FVector2D commandPressedMousePosition;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* moveToEffect;

	UPROPERTY(EditDefaultsOnly)
	float moveToEffectScreenSize = 50;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor moveToEffectColor = FColor::FromHex("#299617");

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void OnSelectPressed();
	void OnSelectReleased();

	void OnCommandPressed(bool bQueue);
	void OnCommandReleased(bool bQueue);

	void OnCommandPressedImmediate();
	void OnCommandReleasedImmediate();

	void OnCommandPressedQueue();
	void OnCommandReleasedQueue();

	virtual void SetupInputComponent() override;

	virtual void SubmitGenericPointOrders(const TArray<AActor*> unitGroup, const FVector& targetLocation, bool bQueue);

	void SpawnMoveToEffect(const FVector2D& screenLocation, const FVector& worldLocation, const FLinearColor& color);

public:
	APlaytimePlayerController();

	void AddToSelection(AActor* actor);

	void ClearSelection();

	UFUNCTION(BlueprintCallable)
	bool DeprojectScreenPositionToLandscape(const FVector2D& screenLocation, FVector& landscapeLocation) const;

	UFUNCTION(BlueprintCallable)
	bool FindUnitsInsideScreenBox(const FVector2D& boxStart, const FVector2D& boxEnd, TArray<AActor*>& outActors) const;

	virtual void SetGenericTeamId(const FGenericTeamId& teamId) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
};
