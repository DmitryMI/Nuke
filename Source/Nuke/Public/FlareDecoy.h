// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/MovementComponent.h"
#include "FlareMovementComponent.h"
#include "FlareDecoy.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFlareDecoyDestroyed, AFlareDecoy*)

UCLASS()
class NUKE_API AFlareDecoy : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float flareDuration = 3.0f;

	UPROPERTY()
	UFlareMovementComponent* flareMovement;

	UPROPERTY()
	FTimerHandle flareDurationHandle;

	FFlareDecoyDestroyed onFlareDecoyDestroyed;

	UFUNCTION()
	void OnFlareDurationExpired();
public:
	// Sets default values for this pawn's properties
	AFlareDecoy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetFlareDuration(float duration);

	UFUNCTION(BlueprintCallable)
	float GetFlareDuration() const;

	UFUNCTION(BlueprintCallable)
	float GetFlareTimeLeft() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetVelocity(const FVector& velocity);

	FFlareDecoyDestroyed& OnFlareDecoyDestroyed();
};
