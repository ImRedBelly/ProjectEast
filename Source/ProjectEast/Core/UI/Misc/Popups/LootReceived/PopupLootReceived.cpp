#include "PopupLootReceived.h"

#include "LootReceivedSlot.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"

void UPopupLootReceived::InitializePopup(UPlayerInventory* InPlayerInventory)
{
	PlayerInventory = InPlayerInventory;
	PlayerInventory->OnItemLooted.AddDynamic(this, &UPopupLootReceived::AddToLooted);
	VerticalBoxLoot->ClearChildren();
}

void UPopupLootReceived::RemoveLootedItem(ULootReceivedSlot* RemoveSlot)
{
	RemoveSlot->RemoveFromParent();
	DisplayNewSlots();
}

void UPopupLootReceived::NativePreConstruct()
{
	Super::NativePreConstruct();
	VerticalBoxLoot->ClearChildren();

	if (IsValid(LootReceivedSlotClass))
		for (int i = 0; i < MaxSlots; ++i)
		{
			ULootReceivedSlot* NewSlot = CreateWidget<ULootReceivedSlot>(this, LootReceivedSlotClass);
			NewSlot->InitializeSlot(this, FText::FromString("ItemName"), 1);
			VerticalBoxLoot->AddChildToVerticalBox(NewSlot);
		}
}

void UPopupLootReceived::NativeDestruct()
{
	if (IsValid(PlayerInventory))
		PlayerInventory->OnItemLooted.RemoveDynamic(this, &UPopupLootReceived::AddToLooted);
	Super::NativeDestruct();
}

void UPopupLootReceived::AddToLooted(FItemData& ItemData)
{
	LootedItems.Add(&ItemData);
	DisplayNewSlots();
}

void UPopupLootReceived::DisplayNewSlots()
{
	for (int i = LootedItems.Num() - 1; i >= 0; --i)
	{
		if (VerticalBoxLoot->GetChildrenCount() > MaxSlots)
			break;
		ULootReceivedSlot* NewSlot = CreateWidget<ULootReceivedSlot>(this, LootReceivedSlotClass);
		NewSlot->InitializeSlot(this, LootedItems[i]->Class.GetDefaultObject()->NameItem,
								LootedItems[i]->Quantity);
		VerticalBoxLoot->AddChildToVerticalBox(NewSlot);
		LootedItems.RemoveAt(i);
	}

}
