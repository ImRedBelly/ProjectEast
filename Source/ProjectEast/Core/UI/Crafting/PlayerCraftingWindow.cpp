#include "PlayerCraftingWindow.h"
#include "CraftingList.h"


void UPlayerCraftingWindow::InitializeWindow(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore)
{
	Super::InitializeWindow(InPlayerCraftingCore, InCraftingCore);
	CraftingList->InitializeList(PlayerCraftingCore, CraftingCore);
}
