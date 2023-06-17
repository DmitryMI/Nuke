// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Attackable.h"
#include "Components/CapsuleComponent.h"
#include "AircraftMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Aircraft.generated.h"

UCLASS()
class NUKE_API AAircraft : public APawn, public IAttackable, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere)
	float maxHealth = 100.0f;

	UPROPERTY(EditAnywhere)
	float health = 100.0f;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere)
	float fuel = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	float bodyLength = 20.0f;
	UPROPERTY(EditDefaultsOnly)
	float bodyRadius = 5.0f;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* bodyCollider;

	UPROPERTY(EditAnywhere)
	UAircraftMovementComponent* aircraftMovement;

	UPROPERTY(EditAnywhere)
	FGenericTeamId teamId;

	UPROPERTY(EditDefaultsOnly)
	float destroyDelay = 5.0f;

	UPROPERTY()
	FTimerHandle destroyDelayedHandle;

	void OnDestroyDelayedExpired();

public:
	// Sets default values for this pawn's properties
	AAircraft();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void DestroyDelayed();


#if WITH_EDITOR  
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool IsAlive() const override;

	virtual void ReceiveDamage(float damageAmount) override;

	void SetGenericTeamId(const FGenericTeamId& team) override;

	FGenericTeamId GetGenericTeamId() const override;

	UAircraftMovementComponent* GetAircraftMovementComponent() const;
};
