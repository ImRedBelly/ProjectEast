#include "ConsumableBarSlot.h"

#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UConsumableBarSlot::NativeConstruct()
{
	Super::NativeConstruct();
	CachedConsumableBuffs = InventoryUtility::GetConsumableBuffs(GetOwningPlayer());
	CachePlayerEquipment = InventoryUtility::GetPlayerEquipment(GetOwningPlayer());
	BindOnItemChanged();
	BindOnRefreshed();
	BindOnUsed();

	//BindGamepad();
}

void UConsumableBarSlot::EventRefreshValues()
{
}

void UConsumableBarSlot::EventClearValues()
{
}

void UConsumableBarSlot::BindOnUsed()
{
}

void UConsumableBarSlot::BindOnRefreshed()
{
}

void UConsumableBarSlot::BindOnItemChanged()
{
}

void UConsumableBarSlot::OnItemUsed()
{
}

void UConsumableBarSlot::OnRefreshItem()
{
}

void UConsumableBarSlot::RefreshSlot()
{
}

void UConsumableBarSlot::GamepadToggled()
{
}

void UConsumableBarSlot::PlayCooldownAnimation()
{
}

void UConsumableBarSlot::SetSlotVisibility()
{
}

bool UConsumableBarSlot::IsAnySlotValid()
{
	return true;
}

bool UConsumableBarSlot::IsUsingGamepad()
{
	return true;
}
