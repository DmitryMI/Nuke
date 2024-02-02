// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitOrder.h"
#include "OrderQueueComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NUKE_API UOrderQueueComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	TArray<UUnitOrder*> orderQueue;

	UPROPERTY(VisibleAnywhere)
	UUnitOrder* currentOrder;

public:	
	// Sets default values for this component's properties
	UOrderQueueComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Enqueue(UUnitOrder* order);

	UUnitOrder* ReportCurrentOrderFinished();

	void SetCurrentOrder(UUnitOrder* order);

	UUnitOrder* GetCurrentOrder() const;

	void PreemptCurrentOrder(UUnitOrder* order);

	void ClearQueue();
};
