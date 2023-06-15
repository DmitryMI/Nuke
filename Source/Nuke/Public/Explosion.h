// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Explosion.generated.h"

UCLASS()
class NUKE_API AExplosion : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	float blastDuration = 0.1f;

	UPROPERTY()
	float lifetime = 5.0f;

	UPROPERTY()
	float blastRadius = 100.0f;

	UPROPERTY()
	float blastDamageMax = 100.0f;

	UPROPERTY()
	bool bIsBlastActive = true;

	UPROPERTY()
	FTimerHandle blastHandle;
	UPROPERTY()
	FTimerHandle lifetimeHandle;

	UPROPERTY(EditAnywhere)
	USphereComponent* blastDamageCollider;

	UFUNCTION()
	void OnBlastExpired();

	UFUNCTION()
	void OnLifetimeExpired();
	
public:	
	// Sets default values for this actor's properties
	AExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBlastDamageCollisionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetBlastDamage(float damage);

};
