// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MobilityEnvironmentType.h"
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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void ReceiveDamage(float damageAmount);

	virtual bool IsAlive() const;

	virtual EMobilityEnvironmentType GetMobilityEnvironmentType() const;
};
