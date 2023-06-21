// Fill out your copyright notice in the Description page of Project Settings.


#include "FlareMovementComponent.h"

void UFlareMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	maxSpeed *= (1 - drag * DeltaTime);

	Velocity += FVector(0, 0, gravity * DeltaTime);
	Velocity = Velocity.GetClampedToMaxSize(maxSpeed * DeltaTime);

	FVector location = GetOwner()->GetActorLocation() + Velocity;
	GetOwner()->SetActorLocation(location);
	UpdateComponentVelocity();
}