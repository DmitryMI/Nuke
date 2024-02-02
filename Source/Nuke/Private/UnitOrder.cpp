// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitOrder.h"

FVector UGenericPointOrder::GetTargetLocation()
{
	return targetLocation;
}

AActor* UGenericActorOrder::GetTargetActor()
{
	return targetActor;
}
