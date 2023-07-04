// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "GUIProxyWidget.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UGUIProxyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UObject> IconImageOverride;

	UPROPERTY(EditAnywhere)
	FText UnitNameOverride;

protected:
	virtual bool Initialize() override;

public:
	
	UFUNCTION(BlueprintNativeEvent)
	void SetIconImage(UObject* image);

	UFUNCTION(BlueprintNativeEvent)
	void SetUnitName(const FText& unitName);

	UFUNCTION(BlueprintNativeEvent)
	void SetColorScheme(const FLinearColor& color);
};
