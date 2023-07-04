// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GUIProxyWidget.h"
#include "FogOfWarComponent.h"
#include "Components/WidgetComponent.h"
#include "PlaytimePlayerController.h"
#include "GUIProxyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NUKE_API UGUIProxyComponent : public UWidgetComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly)
	FLinearColor colorOwnForces = FLinearColor(0, 0.188235294118, 0x8F / 0.560784313725);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor colorFriendlyForces = FLinearColor(0.1, 0.80, 0);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor colorNeutralForces = FColor::FromHex("#808080");

	UPROPERTY(EditDefaultsOnly)
	FLinearColor colorHostileForces = FLinearColor(0.807843137255, 0, 0);

	UFogOfWarComponent* fowComponent;

	APlayerController* localPlayerController;

	bool bIsWidgetVisible = false;

	bool bIsUnitSelected = false;
public:	
	// Sets default values for this component's properties
	UGUIProxyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitWidget() override;

	void ShowWidget();
	void HideWidget();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FVector2D ModifyProjectedLocalPosition(const FGeometry& ViewportGeometry, const FVector2D& LocalPosition) override;

	UFUNCTION(BlueprintCallable)
	bool IsUnitSelectedInGui() const;

	UFUNCTION(BlueprintCallable)
	void SetIsSelectedInGui(bool selectionState);

	UFUNCTION(BlueprintCallable)
	APlaytimePlayerController* GetLocalPlayerController() const;
};
