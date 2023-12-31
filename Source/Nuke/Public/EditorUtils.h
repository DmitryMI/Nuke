#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NavAreas/NavArea.h"
#include "EditorUtils.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UEditorUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Editor", meta = (WorldContext = "WorldContextObject"))
	static void SetWaterNavArea(UObject* worldContext, TSubclassOf<UNavArea> navAreaClass);

	UFUNCTION(BlueprintCallable, Category = "Editor", meta = (WorldContext = "WorldContextObject"))
	static void SetLandNavArea(UObject* worldContext, TSubclassOf<UNavArea> navAreaClass);

	
	UFUNCTION(BlueprintCallable, Category = "Editor")
	static TArray<UClass*> GetAllSubclasses(UClass* clazz);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	static UClass* GetClassByDisplayName(TArray<UClass*> clazzes, const FString& displayName);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	static void SpawnNavModifiersAroundSpline(UObject* worldContext,
		TSubclassOf<AActor> modifierVolumeClass,
		USplineComponent* splineComponent, 
		int subsegmentsNum,
		float width,
		TArray<AActor*> modifierPool, 
		int& modifierPoolIndex, 
		TSubclassOf<UNavArea> navAreaOverride = nullptr
	);

	UFUNCTION(BlueprintCallable, Category = "Editor", meta = (WorldContext = "WorldContextObject"))
	static void SpawnShallowWaterNavModifiers(UObject* worldContext, TSubclassOf<AActor> modifierVolumeClass, TSubclassOf<UNavArea> shallowWaterNavAreaOverride = nullptr);
};
