#include "ConsumableBarSlot.h"

#include "ProjectEast/Core/Components/CharacterSystems/ConsumableBuffs.h"
#include "ProjectEast/Core/Components/Equipment/PlayerEquipment.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/InputDetection/Public/IconButton.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UConsumableBarSlot::NativeConstruct()
{
	Super::NativeConstruct();
	IconButton->Initialize(InputAction);
	CachedConsumableBuffs = InventoryUtility::GetConsumableBuffs(GetOwningPlayer());
	CachePlayerEquipment = InventoryUtility::GetPlayerEquipment(GetOwningPlayer());
	BindOnItemChanged();
	BindOnRefreshed();
	BindOnUsed();
}

void UConsumableBarSlot::EventRefreshValues(FItemData* ItemData)
{
	TextItemName->SetText(ItemData->Class.GetDefaultObject()->NameItem);
	TextItemQuantity->SetText(FText::FromString(FString::FromInt(ItemData->Quantity)));
	ImageItem->SetBrushFromTexture(ItemData->Class.GetDefaultObject()->ImageItem);
	TextX->SetVisibility(ESlateVisibility::Visible);
	TextItemQuantity->SetVisibility(ESlateVisibility::Visible);
	SetSlotVisibility();
}

void UConsumableBarSlot::EventClearValues()
{
	TextItemName->SetText(FText::FromString(EmptySlotText));
	ImageItem->SetBrush(EmptySlotStyle);
	TextX->SetVisibility(ESlateVisibility::Collapsed);
	TextItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	SetSlotVisibility();
}

void UConsumableBarSlot::BindOnUsed()
{
	if (IsValid(CachedConsumableBuffs))
		CachedConsumableBuffs->OnItemUsed.AddDynamic(this, &UConsumableBarSlot::OnItemUsed);
}

void UConsumableBarSlot::BindOnRefreshed()
{
	if (IsValid(CachedConsumableBuffs))
	{
		CachedConsumableBuffs->OnRefresh.AddDynamic(this, &UConsumableBarSlot::RefreshSlot);
		RefreshSlot();
	}
}

void UConsumableBarSlot::BindOnItemChanged()
{
	if (IsValid(CachePlayerEquipment))
	{
		CachePlayerEquipment->OnAddedToEquipment.AddDynamic(this, &UConsumableBarSlot::OnRefreshItem);
		CachePlayerEquipment->OnRemovedFromEquipment.AddDynamic(this, &UConsumableBarSlot::OnRefreshItem);
		RefreshSlot();
	}
}

void UConsumableBarSlot::OnItemUsed(EItemSlot UsedSLot, float Cooldown)
{
	if (UsedSLot == SlotType)
	{
		OnPlayAnimationUse(Cooldown);
		ProgressBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void UConsumableBarSlot::OnRefreshItem(FItemData& ItemData)
{
	RefreshSlot();
}


void UConsumableBarSlot::SetSlotVisibility()
{
	if (HideIfEachEmpty.IsValidIndex(0))
	{
		if (IsAnySlotValid(HideIfEachEmpty))
			SetVisibility(ESlateVisibility::Visible);
		else
			SetVisibility(ESlateVisibility::Hidden);
	}
	else
		SetVisibility(ESlateVisibility::Visible);
}

bool UConsumableBarSlot::IsAnySlotValid(TArray<EItemSlot> Array) const
{
	for (auto ItemSlot : Array)
	{
		auto Data = CachePlayerEquipment->GetItemByEquipmentSlot(ItemSlot);
		if (Data.Get<0>())
			return true;
	}
	return false;
}

void UConsumableBarSlot::RefreshSlot(FItemData& ItemData)
{
	RefreshSlot();
}

void UConsumableBarSlot::RefreshSlot()
{
	auto Data = CachePlayerEquipment->GetItemByEquipmentSlot(SlotType);
	if (Data.Get<0>())
		EventRefreshValues(Data.Get<1>());
	else
		EventClearValues();
}
