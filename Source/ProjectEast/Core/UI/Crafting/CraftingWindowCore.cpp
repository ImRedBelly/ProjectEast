#include "CraftingWindowCore.h"

#include "CraftingBase.h"
#include "CraftingList.h"

void UCraftingWindowCore::InitializeWindow(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore)
{
	PlayerCraftingCore = InPlayerCraftingCore;
	CraftingCore = InCraftingCore;

	CraftingBase->InitializeCraftingBase(PlayerCraftingCore, CraftingCore);
	CraftingList->InitializeList(PlayerCraftingCore, CraftingCore);
}
