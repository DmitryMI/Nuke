// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIProxyWidget.h"
#include "GUIProxyComponent.h"



bool UGUIProxyWidget::Initialize()
{
	bool ok = Super::Initialize();
	if (!ok)
	{
		return false;
	}

	SetIconImage(IconImageOverride);
	SetUnitName(UnitNameOverride);
	return true;
}

void UGUIProxyWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsMouseInside = true;
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	//UE_LOG(LogTemp, Display, TEXT("NativeOnMouseEnter, bIsMouseInside: %d"), (int)bIsMouseInside);
}

void UGUIProxyWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	bIsMouseInside = false;
	Super::NativeOnMouseLeave(InMouseEvent);
	//UE_LOG(LogTemp, Display, TEXT("NativeOnMouseLeave bIsMouseInside: %d"), (int)bIsMouseInside);
}

FReply UGUIProxyWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//owningComponent->GetLocalPlayerController()->OnGuiProxyWidgetClicked(this);

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FLinearColor UGUIProxyWidget::GetModifiedBaseColor() const
{
	FLinearColor translucent = baseColor;
	translucent.A = foregroundOpacity;

	if (bIsMouseInside)
	{
		translucent += FLinearColor(0.2, 0.2, 0.2, 0.2);
	}
	if (owningComponent->IsUnitSelectedInGui())
	{
		translucent += FLinearColor(0, 0.2, 0, 0.2);
	}

	return translucent;
}


void UGUIProxyWidget::SetColorSchemeNative(const FLinearColor& color)
{
	baseColor = color;
	OnColorSchemeSet(color);
}

void UGUIProxyWidget::SetSelectionStateNative(bool selectionState)
{
	OnSelectionStateSet(selectionState);
}

UGUIProxyComponent* UGUIProxyWidget::GetOwningGuiProxy() const
{
	return owningComponent;
}

void UGUIProxyWidget::SetOwningGuiProxy(UGUIProxyComponent* owner)
{
	owningComponent = owner;
}


void UGUIProxyWidget::OnSelectionStateSet_Implementation(bool selectionState)
{
}

void UGUIProxyWidget::SetIconImage_Implementation(UObject* image)
{
}

void UGUIProxyWidget::SetUnitName_Implementation(const FText& unitName)
{
}

void UGUIProxyWidget::OnColorSchemeSet_Implementation(const FLinearColor& color)
{

}