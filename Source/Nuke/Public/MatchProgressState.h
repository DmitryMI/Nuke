// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class EMatchProgressState : uint8 {
	MPS_WaitingForStart = 0		UMETA(DisplayName = "WaitingForStart"),
	MPS_Deployment = 1			UMETA(DisplayName = "Deployment"),
	MPS_Wargame = 2				UMETA(DisplayName = "Wargame")
};
