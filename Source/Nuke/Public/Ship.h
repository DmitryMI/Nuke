// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShipMovementComponent.h"
#include "Ship.generated.h"

UCLASS()
class NUKE_API AShip : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UShipMovementComponent* shipMovement;
public:
	// Sets default values for this pawn's properties
	AShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
