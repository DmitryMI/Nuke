// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShipMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Attackable.h"
#include "Ship.generated.h"

UCLASS()
class NUKE_API AShip : public APawn, public IGenericTeamAgentInterface, public IAttackable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UShipMovementComponent* shipMovement;

	UPROPERTY(EditAnywhere)
	FGenericTeamId teamId;

public:
	// Sets default values for this pawn's properties
	AShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void SetGenericTeamId(const FGenericTeamId& team) override;
};
