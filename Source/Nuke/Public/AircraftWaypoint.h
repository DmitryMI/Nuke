#pragma once

#include "CoreMinimal.h"
#include "AircraftWaypoint.generated.h"

USTRUCT(BlueprintType)
struct FAircraftWaypoint
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    bool bIsValid = false;

    UPROPERTY(EditAnywhere)
    FVector Location;

    UPROPERTY(EditAnywhere)
    float Speed;

    FAircraftWaypoint();

    FAircraftWaypoint(const FVector& location, float speed);
};
