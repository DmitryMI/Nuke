// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "AircraftWaypoint.h"
#include "AircraftMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UAircraftMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	FVector2D maxAngularSpeedsPitchYaw = FVector2D(90, 90);

	UPROPERTY(EditDefaultsOnly)
	FVector2D minAngularSpeedsPitchYaw = FVector2D(20, 20);

	UPROPERTY(EditDefaultsOnly)
	FVector maxVisualRotationSpeed = FVector(90, 90, 90);

	UPROPERTY(EditDefaultsOnly)
	float maxRollAngleDeg = 60.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector2D pitchMinMax = FVector2D(-45, 45);

	UPROPERTY(EditDefaultsOnly)
	float acceleration = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float deceleration = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector2D speedMinMax = FVector2D(500, 1500);

	UPROPERTY(VisibleAnywhere)
	float currentSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere)
	FRotator requestedRotation;

	UPROPERTY(VisibleAnywhere)
	float requestedSpeed = 1500;

	UPROPERTY(VisibleAnywhere)
	FAircraftWaypoint requestedWaypoint;

	UPROPERTY(VisibleAnywhere)
	bool bNavigateToWaypoint = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsFreeFalling = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsMovementFrozen = false;

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateMovementSpeed(float DeltaTime);

	void UpdateActorRotation(const FRotator& toRotation, float DeltaTime);

	FRotator RotateTowardsTarget(float DeltaTime);

	template<typename T>
	T ClampAbs(const T& value, const T& absLimit);

public:
	UFUNCTION(BlueprintCallable)
	void RequestSpeed(float speed);

	UFUNCTION(BlueprintCallable)
	void RequestRotation(float pitch, float yaw);

	UFUNCTION(BlueprintCallable)

	void RequestFlyTowardsWaypoint(const FAircraftWaypoint& waypoint);

	bool GetRequestedWaypoint(FAircraftWaypoint& outWaypoint) const;

	float GetCurrentSpeed() const;

	void FreezeMovement();

	virtual float GetMaxSpeed() const override;
};

template<typename T>
inline T UAircraftMovementComponent::ClampAbs(const T& value, const T& absLimit)
{
	if (value < 0)
	{
		return FMath::Max(-absLimit, value);
	}
	else if (value > 0)
	{
		return FMath::Min(absLimit, value);
	}
	else
	{
		return value;
	}
}
