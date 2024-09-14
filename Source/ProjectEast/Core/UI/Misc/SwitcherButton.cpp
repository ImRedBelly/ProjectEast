#include "SwitcherButton.h"
#include "Components/Button.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"

void USwitcherButton::InitializeButton(UWidgetManager* WidgetManager)
{
	CachedWidgetManager = WidgetManager;
	SwitchButton->OnClicked.AddDynamic(this, &USwitcherButton::OnClickedButton);
	SwitchButton->OnHovered.AddDynamic(this, &USwitcherButton::OnHoveredButton);
	SwitchButton->OnUnhovered.AddDynamic(this, &USwitcherButton::OnUnHoveredButton);

	if (CachedWidgetManager)
		CachedWidgetManager->OnSwitchTab.AddDynamic(this, &USwitcherButton::OnUpdateViewButton);

	OnUnHoveredButton();
}

void USwitcherButton::NativeDestruct()
{
	Super::NativeDestruct();
	SwitchButton->OnClicked.RemoveDynamic(this, &USwitcherButton::OnClickedButton);
	SwitchButton->OnHovered.RemoveDynamic(this, &USwitcherButton::OnHoveredButton);
	SwitchButton->OnUnhovered.RemoveDynamic(this, &USwitcherButton::OnUnHoveredButton);

	if (CachedWidgetManager)
		CachedWidgetManager->OnSwitchTab.RemoveDynamic(this, &USwitcherButton::OnUpdateViewButton);
}

void USwitcherButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	TextNameButton->SetText(NameButton);
}

void USwitcherButton::OnClickedButton()
{
	if (CachedWidgetManager)
	{
		if (bOpenWidget)
			CachedWidgetManager->SwitchWidgetTo(WidgetType);
		else
			CachedWidgetManager->SwitchTabTo(WidgetType);
	}
}

void USwitcherButton::OnHoveredButton()
{
	if (GetActiveTab() != WidgetType)
		SwitchButton->SetColorAndOpacity(HoveredColor);
}

void USwitcherButton::OnUnHoveredButton()
{
	if (GetActiveTab() != WidgetType)
		SwitchButton->SetColorAndOpacity(UnHoveredColor);
}

EWidgetType USwitcherButton::GetActiveTab()
{
	if (CachedWidgetManager)
		return CachedWidgetManager->GetActiveTab();
	return EWidgetType::None;
}

void USwitcherButton::OnUpdateViewButton(EWidgetType Widget)
{
	if (Widget == WidgetType)
		SwitchButton->SetColorAndOpacity(NormalColor);
	else
		SwitchButton->SetColorAndOpacity(UnHoveredColor);
}
