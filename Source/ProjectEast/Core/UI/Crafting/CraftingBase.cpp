#include "CraftingBase.h"

#include "CraftingMaterialsBar.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingBase::NativeConstruct()
{
	Super::NativeConstruct();
	CraftingMaterialsBar->InitializeWidget(InventoryUtility::GetPlayerCrafting(GetOwningPlayer()));
}
