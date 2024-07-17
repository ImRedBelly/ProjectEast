#include "PlayerEquipment.h"

#include "Kismet/BlueprintMapLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UPlayerEquipment::ServerTransferItemFromInventory(FItemData* ItemData, FItemData* InSlotData,
                                                       UPlayerInventory* PlayerInventory, EInputMethodType RightClick)
{
}

void UPlayerEquipment::RemoveItemFromEquipmentArray(FItemData* ItemData)
{
}

void UPlayerEquipment::DetachItemFromEquipment(FItemData* ItemData)
{
}

bool UPlayerEquipment::CanItemBeEquipped(FItemData* ItemData)
{
	return true;
}

TTuple<bool, FItemData*> UPlayerEquipment::GetItemByEquipmentSlot(EItemSlot Slot) const
{
	if (EquipmentData.Contains(Slot))
	{
		FItemData* ItemData = EquipmentData.FindRef(Slot);
		if (InventoryUtility::IsItemClassValid(ItemData))
			return MakeTuple(true, ItemData);
	}
	return MakeTuple(false, nullptr);
}
