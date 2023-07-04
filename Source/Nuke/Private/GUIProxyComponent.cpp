// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIProxyComponent.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"

// Sets default values for this component's properties
UGUIProxyComponent::UGUIProxyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(100, 25));
}


// Called when the game starts
void UGUIProxyComponent::BeginPlay()
{
	Super::BeginPlay();

	fowComponent = GetOwner()->GetComponentByClass<UFogOfWarComponent>();
	localPlayerController = GetWorld()->GetFirstPlayerController();
}

void UGUIProxyComponent::ShowWidget()
{
	UGUIProxyWidget* widget = Cast<UGUIProxyWidget>(GetWidget());
	if (!widget)
	{
		return;
	}

	if (bIsWidgetVisible)
	{
		return;
	}

	bIsWidgetVisible = true;

	FLinearColor widgetColor = colorNeutralForces;

	IGenericTeamAgentInterface* teamAgent = GetOwner<IGenericTeamAgentInterface>();
	check(teamAgent);

	if (localPlayerController)
	{
		APlaytimePlayerState* localPlayerState = localPlayerController->GetPlayerState<APlaytimePlayerState>();
		check(localPlayerState);

		FGenericTeamId localPlayerTeam = localPlayerState->GetGenericTeamId();

		FGenericTeamId actorTeam = teamAgent->GetGenericTeamId();

		if (localPlayerTeam == actorTeam)
		{
			widgetColor = colorOwnForces;
		}
		else
		{
			auto attitude = FGenericTeamId::GetAttitude(localPlayerTeam, actorTeam);
			if (attitude == ETeamAttitude::Friendly)
			{
				widgetColor = colorFriendlyForces;
			}
			else if (attitude == ETeamAttitude::Hostile)
			{
				widgetColor = colorHostileForces;
			}
		}
	}

	widget->SetColorSchemeNative(widgetColor);
	widget->SetVisibility(ESlateVisibility::Visible);
}

void UGUIProxyComponent::HideWidget()
{
	UGUIProxyWidget* widget = Cast<UGUIProxyWidget>(GetWidget());
	if (!widget)
	{
		return;
	}

	if (!bIsWidgetVisible)
	{
		return;
	}

	bIsWidgetVisible = false;

	widget->SetVisibility(ESlateVisibility::Collapsed);
}


// Called every frame
void UGUIProxyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (fowComponent)
	{
		if (fowComponent->IsVisibleForController(localPlayerController))
		{
			ShowWidget();
		}
		else
		{
			HideWidget();
		}
	}
	else
	{
		ShowWidget();
	}
}

void UGUIProxyComponent::InitWidget()
{
	Super::InitWidget();

	UGUIProxyWidget* widget = Cast<UGUIProxyWidget>(GetWidget());
	if (!widget)
	{
		return;
	}

	widget->SetOwningGuiProxy(this);
}

bool UGUIProxyComponent::IsUnitSelectedInGui() const
{
	return bIsUnitSelected;
}

void UGUIProxyComponent::SetIsSelectedInGui(bool selectionState)
{
	bIsUnitSelected = selectionState;

	if (bIsUnitSelected)
	{
		UE_LOG(LogTemp, Display, TEXT("%s added to selection"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("%s removed from selection"), *GetOwner()->GetName());
	}

	UGUIProxyWidget* widget = Cast<UGUIProxyWidget>(GetWidget());
	if (!widget)
	{
		return;
	}

	widget->SetSelectionStateNative(selectionState);
}

FVector2D UGUIProxyComponent::ModifyProjectedLocalPosition(const FGeometry& ViewportGeometry, const FVector2D& LocalPosition)
{
	lastScreenLocation = LocalPosition;
	return LocalPosition;
}

APlaytimePlayerController* UGUIProxyComponent::GetLocalPlayerController() const
{
	return Cast<APlaytimePlayerController>(localPlayerController);
}

bool UGUIProxyComponent::GetWidgetScreenLocation(FVector2D& location) const
{
	UGUIProxyWidget* widget = Cast<UGUIProxyWidget>(GetWidget());
	if (!widget || widget->GetVisibility() != ESlateVisibility::Visible)
	{
		return false;
	}

	location = lastScreenLocation;
	return true;
}

bool UGUIProxyComponent::IsWidgetVisible() const
{
	UGUIProxyWidget* widget = Cast<UGUIProxyWidget>(GetWidget());
	if (!widget || widget->GetVisibility() != ESlateVisibility::Visible)
	{
		return false;
	}
	return true;
}