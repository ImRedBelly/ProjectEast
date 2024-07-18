#include "UpperUIBar.h"

#include "Components/Button.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UUpperUIBar::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonClose->OnClicked.AddDynamic(this, &UUpperUIBar::CloseInventory);
}

void UUpperUIBar::CloseInventory()
{
	InventoryUtility::GetPlayerInventory(GetOwningPlayer())->InputCloseWidget();
}
