// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIProxyWidget.h"

void UGUIProxyWidget::SetColorScheme_Implementation(const FLinearColor& color)
{

}

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

void UGUIProxyWidget::SetIconImage_Implementation(UObject* image)
{
}

void UGUIProxyWidget::SetUnitName_Implementation(const FText& unitName)
{
}
