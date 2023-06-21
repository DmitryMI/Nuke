// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RadarDetectorComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FRadarDetectorNotification, URadarDetectorComponent*, AActor*, FRadarDetectionInfo);

USTRUCT(BlueprintType)
struct FRadarDetectionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	AActor* Irradiator;

	UPROPERTY(EditAnywhere)
	float DetectionTimestamp;

	UPROPERTY(EditAnywhere)
	float UpdateTimestamp;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NUKE_API URadarDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	TMap<AActor*, FRadarDetectionInfo> detectedIrradiators;

	UPROPERTY(VisibleAnywhere)
	UPrimitiveComponent* pawnCollisionComponent;

	FRadarDetectorNotification onRadarDetectorNotification;
public:	
	// Sets default values for this component's properties
	URadarDetectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	FRadarDetectorNotification& OnRadarDetectorNotification();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void NotifyDetection(const AActor* sender);

	UFUNCTION(BlueprintCallable)
	void ClearDetectionData();

	UFUNCTION(BlueprintCallable)
	bool GetDetectedRadars(TArray<AActor*>& outRadarActors) const;

	template<typename T>
	bool GetDetectedRadars(TArray<T*>& outRadarActors) const
	{
		for (auto pair : detectedIrradiators)
		{
			T* actorT = Cast<T>(pair.Key);
			if (!actorT)
			{
				continue;
			}

			outRadarActors.Add(actorT);
		}

		return outRadarActors.Num() > 0;
	}
};
