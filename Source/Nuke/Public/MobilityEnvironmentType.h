#pragma once

#include "MobilityEnvironmentType.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMobilityEnvironmentType : uint8 {
	MET_None = 0				UMETA(Hidden),
	MET_Ground = (1 << 0)		UMETA(DisplayName = "Ground"),
	MET_Air = (1 << 1)			UMETA(DisplayName = "Air"),
	MET_WaterSurface = (1 << 2)	UMETA(DisplayName = "WaterSurface"),
	MET_Underwater = (1 << 3)	UMETA(DisplayName = "Underwater"),
};

ENUM_CLASS_FLAGS(EMobilityEnvironmentType);