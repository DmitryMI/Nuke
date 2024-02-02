// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderQueueComponent.h"

// Sets default values for this component's properties
UOrderQueueComponent::UOrderQueueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UOrderQueueComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOrderQueueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOrderQueueComponent::Enqueue(UUnitOrder* order)
{
	orderQueue.Add(order);
}

UUnitOrder* UOrderQueueComponent::ReportCurrentOrderFinished()
{
	if (orderQueue.Num() > 0)
	{
		currentOrder = orderQueue[0];
		orderQueue.RemoveAt(0);
		return currentOrder;
	}
	else
	{
		currentOrder = nullptr;
		return nullptr;
	}
}

UUnitOrder* UOrderQueueComponent::GetCurrentOrder() const
{
	return currentOrder;
}

void UOrderQueueComponent::PreemptCurrentOrder(UUnitOrder* order)
{
	if (!currentOrder)
	{
		currentOrder = order;
	}
	else
	{
		orderQueue.Insert(currentOrder, 0);
		currentOrder = order;
	}
}

void UOrderQueueComponent::SetCurrentOrder(UUnitOrder* order)
{
	currentOrder = order;
}

void UOrderQueueComponent::ClearQueue()
{
	orderQueue.Empty();
}

