#include "PlayerEquipment.h"

#include "Kismet/BlueprintMapLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UPlayerEquipment::BeginPlay()
{
	Super::BeginPlay();
	BuildEquipment();
}

void UPlayerEquipment::BuildEquipment()
{
	for (int32 i = 0; i <= static_cast<int32>(EItemSlot::Pocket4); ++i)
	{
		EItemSlot EnumValue = static_cast<EItemSlot>(i);
		auto NewItemData = new FItemData();
		NewItemData->EquipmentSlot = EnumValue;
		EquipmentData.Add(EnumValue, NewItemData);
	}
}

void UPlayerEquipment::ServerTransferItemFromInventory(FItemData* ItemData, FItemData* InSlotData,
                                                       UInventoryCore* PlayerInventory, EInputMethodType RightClick)
{
}

void UPlayerEquipment::ServerTransferItemFromEquipment(FItemData* ItemData, FItemData* CurrentItemData)
{
}

void UPlayerEquipment::RemoveItemFromEquipmentArray(FItemData* ItemData)
{
}

void UPlayerEquipment::AddToStackInEquipment(FItemData* ItemData, FItemData* Element)
{
}

bool UPlayerEquipment::TryToAddToPartialStack(FItemData* ItemData)
{
	TArray<FItemData*> ItemsData;
	EquipmentData.GenerateValueArray(ItemsData);
	auto PartialStackData = InventoryUtility::HasPartialStack(ItemsData, ItemData);

	if (PartialStackData.Get<0>())
	{
		AddToStackInEquipment(ItemData, ItemsData[PartialStackData.Get<1>()]);
		return true;
	}

	return false;
}

void UPlayerEquipment::DetachItemFromEquipment(FItemData* ItemData)
{
}

TTuple<bool, FText> UPlayerEquipment::CanItemBeEquipped(FItemData* ItemData)
{
	if(bIsCheckRequiredLevel)
	{
		//GetComponent<ACLeveling>()
		//if(!Leveling)
		//	return MakeTuple(false, FText::FromString("Higher Level required."));
		
	}
	return MakeTuple(true, FText());
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
