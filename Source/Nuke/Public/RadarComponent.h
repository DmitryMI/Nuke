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
	float trackingRange = 10000.0f;

	UPROPERTY(EditAnywhere)
	float visibilityRange = 10000.0f;

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EMobilityEnvironmentType))
	int32 trackableMobilityFlags = 0;

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EMobilityEnvironmentType))
	int32 visibleMobilityFlags = 0;

	UPROPERTY(EditAnywhere)
	bool bNotifyRadarDetectors = true;

public:	
	// Sets default values for this component's properties
	URadarComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BindPrimitiveComponentColliders();

	virtual void OnActorEnteredRadarRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp);

	virtual void ProcessInitialOverlap(UPrimitiveComponent* OverlappedComponent, AActor* actor, UPrimitiveComponent* otherComponent);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual bool TryTrackAndNotify(AActor* actor) const;

	virtual bool IsRadarDetectorNotificationEnabled() const;

	virtual void SetRadarDetectorNotificationEnabled(bool enabled);

	virtual void UpdateVisibilityOfActorsInRange();

	TArray<AActor*>& GetActorsInRadarRange();

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
	virtual void SetTrackingRange(float range);

	UFUNCTION(BlueprintCallable)
	float GetTrackingRange() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetVisibilityRange(float range);

	UFUNCTION(BlueprintCallable)
	float GetVisibilityRange() const;

	UFUNCTION(BlueprintCallable)
	float GetRadarCollisionRadius() const;

	int32 GetTrackableMobilityFlags() const;

	int32 GetVisibleMobilityFlags() const;

	void SetTrackableMobilityFlags(int32 flags);

	void SetVisibleMobilityFlags(int32 flags);
};
