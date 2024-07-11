#include "PlayerEquipment.h"

void UPlayerEquipment::ServerTransferItemFromInventory(FItemData* ItemData, FItemData* InSlotData,
	UPlayerInventory* PlayerInventory, EInputMethodType RightClick)
{
}

bool UPlayerEquipment::CanItemBeEquipped(FItemData* ItemData)
{
	return true;
}
