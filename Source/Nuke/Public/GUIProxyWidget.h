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

	UPROPERTY(EditAnywhere)
	float foregroundOpacity = 0.5f;

	UGUIProxyComponent* owningComponent;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor baseColor;

	UPROPERTY(BlueprintReadWrite)
	bool bIsMouseInside = false;

protected:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnColorSchemeSet(const FLinearColor& color);

	UFUNCTION(BlueprintNativeEvent)
	void OnSelectionStateSet(bool selectionState);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
public:

	UFUNCTION(BlueprintCallable)
	FLinearColor GetModifiedBaseColor() const;
	
	UFUNCTION(BlueprintNativeEvent)
	void SetIconImage(UObject* image);

	UFUNCTION(BlueprintNativeEvent)
	void SetUnitName(const FText& unitName);

	void SetColorSchemeNative(const FLinearColor& color);
	void SetSelectionStateNative(bool selectionState);

	UFUNCTION(BlueprintCallable)
	class UGUIProxyComponent* GetOwningGuiProxy() const;

	UFUNCTION(BlueprintCallable)
	void SetOwningGuiProxy(class UGUIProxyComponent* owner);

};
