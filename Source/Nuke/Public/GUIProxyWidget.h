// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GUIProxyWidget.generated.h"

/**
 * 
 */
UCLASS()
class NUKE_API UGUIProxyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetColorScheme(const FLinearColor& color);
};
