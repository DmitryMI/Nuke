// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Radar.h"
#include "MobilityEnvironmentType.h"
#include "RadarComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NUKE_API URadarComponent : public USceneComponent, public IGenericTeamAgentInterface, public IRadar
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> threatsInRadarRange;

	UPROPERTY(EditAnywhere)
	EMobilityEnvironmentType detectableMobilityType = EMobilityEnvironmentType::MET_None;

public:	
	// Sets default values for this component's properties
	URadarComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BindPrimitiveComponentColliders();

	virtual void OnActorEnteredRadarRage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp);

	virtual void ProcessInitialOverlap(UPrimitiveComponent* OverlappedComponent, AActor* actor, UPrimitiveComponent* otherComponent);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual bool EvaluateTrackingConditions(AActor* actor) const;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<AActor*> GetTrackedThreatsArray() const;

	virtual bool GetTrackedThreats(TArray<AActor*>& outThreats) const override;

	UFUNCTION(BlueprintCallable)
	virtual void SetGenericTeamId(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsActorTrackedByRadar(AActor* actor) const override;

	UFUNCTION(BlueprintCallable)
	virtual EMobilityEnvironmentType GetDetectableMobilityType() const;

	UFUNCTION(BlueprintCallable)
	void SetDetectableMobilityType(EMobilityEnvironmentType mobilityType);
};
