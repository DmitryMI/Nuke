// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MobilityEnvironmentType.h"
#include "FogOfWarComponent.h"
#include "Attackable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UAttackable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NUKE_API IAttackable
{
	GENERATED_BODY()

public:

	virtual void ReceiveDamage(float damageAmount);

	virtual bool IsAlive() const;

	virtual EMobilityEnvironmentType GetMobilityEnvironmentType() const;

	virtual float GetStealthFactor() const;

	virtual UFogOfWarComponent* GetFogOfWarComponent() const;
};
