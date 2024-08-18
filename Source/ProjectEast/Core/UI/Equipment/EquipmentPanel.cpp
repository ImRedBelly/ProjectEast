#include "EquipmentSlot.h"
#include "EquipmentPanel.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"

void UEquipmentPanel::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerEquipment = InventoryUtility::GetPlayerEquipment(GetOwningPlayer());
	CachedPlayerInventory = InventoryUtility::GetPlayerInventory(GetOwningPlayer());

	CreateEquipment();
	BindEventDispatchers();
}

void UEquipmentPanel::NativeDestruct()
{
	UnbindEventDispatchers();
	Super::NativeDestruct();
}

FReply UEquipmentPanel::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToSlot(FocusedSlot);
	return FReply::Handled();
}

void UEquipmentPanel::SetFocusToSlot(EItemSlot ItemSlot)
{
	auto EquipmentSlot = GetWidgetSlotByItemSlot(ItemSlot);
	if (IsValid(EquipmentSlot))
	{
		FocusedSlot = ItemSlot;
		EquipmentSlot->SetKeyboardFocus();
	}
}

void UEquipmentPanel::EmptyEquipmentSlot(FItemData& ItemData)
{
	FItemData* LocalItemData = &ItemData;
	if (InventoryUtility::IsItemClassValid(LocalItemData))
		GetWidgetSlotByItemSlot(LocalItemData->EquipmentSlot)->EmptySlot();
}

void UEquipmentPanel::OverwriteEquipmentSlot(FItemData& ItemData)
{
	FItemData* LocalItemData = &ItemData;
	if (InventoryUtility::IsItemClassValid(LocalItemData))
		GetWidgetSlotByItemSlot(LocalItemData->EquipmentSlot)->OverwriteSlot(this, LocalItemData);
}

void UEquipmentPanel::AssignCurrentlyFocusedSlot(EItemSlot ItemSlot)
{
	FocusedSlot = ItemSlot;
}

UEquipmentSlot* UEquipmentPanel::GetWidgetSlotByItemSlot(EItemSlot ItemSlot) const
{
	switch (ItemSlot)
	{
	case EItemSlot::Head:
		return HeadSlot;
	case EItemSlot::Gloves:
		return GlovesSlot;
	case EItemSlot::Neck:
		return NecklaceSlot;
	case EItemSlot::Chest:
		return ChestPlateSlot;
	case EItemSlot::Legs:
		return LegsSlot;
	case EItemSlot::Boots:
		return BootSlot;
	case EItemSlot::Weapon:
		return WeaponSlot;
	case EItemSlot::Shield:
		return ShieldSlot;
	case EItemSlot::Pocket1:
		return Pot1Slot;
	case EItemSlot::Pocket2:
		return Pot2Slot;
	case EItemSlot::Pocket3:
		return Pot3Slot;
	case EItemSlot::Pocket4:
		return Pot4Slot;
	default:
		return nullptr;
	}
}

void UEquipmentPanel::CreateEquipment()
{
	TArray<EItemSlot> Slots;
	CachedPlayerEquipment->GetEquipmentData().GetKeys(Slots);
	for (int i = 0; i < Slots.Num(); ++i)
	{
		auto FindItemSlot = GetWidgetSlotByItemSlot(Slots[i]);
		auto FindItem = CachedPlayerEquipment->GetEquipmentData().Find(Slots[i]);

		if (IsValid(FindItemSlot))
			FindItemSlot->OverwriteSlot(this, *FindItem);
	}
}

void UEquipmentPanel::BindEventDispatchers()
{
	CachedPlayerEquipment->OnAddedToEquipment.AddDynamic(this, &UEquipmentPanel::OverwriteEquipmentSlot);
	CachedPlayerEquipment->OnRemovedFromEquipment.AddDynamic(this, &UEquipmentPanel::EmptyEquipmentSlot);
}

void UEquipmentPanel::UnbindEventDispatchers()
{
	if (IsValid(CachedPlayerEquipment))
	{
		CachedPlayerEquipment->OnAddedToEquipment.RemoveDynamic(this, &UEquipmentPanel::OverwriteEquipmentSlot);
		CachedPlayerEquipment->OnRemovedFromEquipment.RemoveDynamic(this, &UEquipmentPanel::EmptyEquipmentSlot);
	}
}
