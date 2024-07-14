#include "PlayerEquipment.h"

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
