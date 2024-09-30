#include "InventoryWindow.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"

void UInventoryWindow::CloseWidget()
{
	Cast<AMainPlayerController>(GetOwningPlayer())->GetPlayerInventory()->InputCloseWidget();
}
