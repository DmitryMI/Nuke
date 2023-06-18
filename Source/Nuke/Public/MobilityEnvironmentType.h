#pragma once

#include "MobilityEnvironmentType.generated.h"

UENUM(BlueprintType)
enum class EMobilityEnvironmentType : uint8 {
	MET_None = 0			UMETA(DisplayName = "None"),
	MET_Ground = 1			UMETA(DisplayName = "Ground"),
	MET_Air = 2				UMETA(DisplayName = "Air"),
	MET_WaterSurface = 3	UMETA(DisplayName = "WaterSurface"),
	MET_Underwater = 4		UMETA(DisplayName = "Underwater"),
	MET_MAX
};