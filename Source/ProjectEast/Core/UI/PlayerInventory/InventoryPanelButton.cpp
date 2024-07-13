#include "InventoryPanelButton.h"

#include "Components/Border.h"
#include "Components/Button.h"

void UInventoryPanelButton::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonPanel->OnClicked.AddDynamic(this, &UInventoryPanelButton::ButtonOnClicked);
}

void UInventoryPanelButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	ButtonPanel->SetStyle(ButtonStyleOff);
}

void UInventoryPanelButton::ButtonOnClicked()
{
	if (OnClickedButtonPanel.IsBound())
		OnClickedButtonPanel.Broadcast(Panel);
}

void UInventoryPanelButton::RefreshPanelButton(EInventoryPanels ActivePanel) const
{
	BorderBackground->SetBrushColor(FLinearColor());
	ButtonPanel->SetStyle(ButtonStyleOff);
	if (Panel == ActivePanel)
	{
		BorderBackground->SetBrushColor(FLinearColor(1, 1, 1, 1));
		ButtonPanel->SetStyle(ButtonStyleOn);
	}
}
