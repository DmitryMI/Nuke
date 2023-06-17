#pragma once

#include "CoreMinimal.h"
#include "AircraftWaypoint.generated.h"

USTRUCT(BlueprintType)
struct FAircraftWaypoint
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    FVector Location;

    UPROPERTY(EditAnywhere)
    float Speed;
};
