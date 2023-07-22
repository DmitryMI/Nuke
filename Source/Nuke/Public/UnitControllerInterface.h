// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnitControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUnitControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NUKE_API IUnitControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual bool IssueGenericPointOrder(const FVector& location, bool queue = false);
	virtual bool IssueGenericActorOrder(AActor* actor, bool queue = false);
	virtual bool IssueStopOrder();
};
