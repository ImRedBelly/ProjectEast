#include "CraftingListSwitchButton.h"

void UCraftingListSwitchButton::RefreshListButton(ECraftingCategory NewCategory)
{
	bIsActive = false;
	ButtonSwitch->SetStyle(ButtonStyleOff);
	if (NewCategory == Category)
	{
		ButtonSwitch->SetStyle(ButtonStyleOn);
		bIsActive = true;
	}
}

ECraftingCategory UCraftingListSwitchButton::GetCraftingCategory() const
{
	return Category;
}

void UCraftingListSwitchButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	ButtonSwitch->SetStyle(ButtonStyleOff);
	if (bIsActive)
		RefreshListButton(Category);
}

void UCraftingListSwitchButton::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonSwitch->OnClicked.AddDynamic(this, &UCraftingListSwitchButton::OnClicked);
}

void UCraftingListSwitchButton::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonSwitch->OnClicked.RemoveDynamic(this, &UCraftingListSwitchButton::OnClicked);
}

void UCraftingListSwitchButton::OnClicked()
{
	if (OnClickedButton.IsBound())
		OnClickedButton.Broadcast(Category);
}
