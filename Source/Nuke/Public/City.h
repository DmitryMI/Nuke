// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "GenericTeamAgentInterface.h"
#include "City.generated.h"

UCLASS()
class NUKE_API ACity : public APawn, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	FText cityName;

	UPROPERTY(EditAnywhere)
	int population;

	UPROPERTY(EditAnywhere)
	float radius;

	UPROPERTY(EditAnywhere, Category = "Team")
	FGenericTeamId teamId = FGenericTeamId::NoTeam;
	
	UPROPERTY(EditAnywhere)
	USphereComponent* cityRadiusCollider;

public:
	// Sets default values for this pawn's properties
	ACity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetGenericTeamId(const FGenericTeamId& team);

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const;


};
