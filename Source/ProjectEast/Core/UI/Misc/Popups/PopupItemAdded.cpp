#include "PopupItemAdded.h"

#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UPopupItemAdded::InitializeWidget(FItemData* ItemData)
{
	CurrentItemData = ItemData;
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		SetItemImage();
		SetItemName();
		SetItemType();
		SetItemRarity();
		SetItemRarityColor();
		PlayConstructAnimation();
	}
}

void UPopupItemAdded::SetItemImage() const
{
	ImageItem->SetBrushFromTexture(CurrentItemData->Class.GetDefaultObject()->ImageItem);
}

void UPopupItemAdded::SetItemName() const
{
	TextItemName->SetText(CurrentItemData->Class.GetDefaultObject()->NameItem);
}

void UPopupItemAdded::SetItemType() const
{
	TextItemName->SetText(UEnum::GetDisplayValueAsText(CurrentItemData->Class.GetDefaultObject()->Type));
}

void UPopupItemAdded::SetItemRarity() const
{
	TextItemName->SetText(UEnum::GetDisplayValueAsText(CurrentItemData->Class.GetDefaultObject()->Rarity));
}

void UPopupItemAdded::SetItemRarityColor() const
{
	auto Color = InventoryUtility::GetRarityColor(CurrentItemData->Class.GetDefaultObject()->Rarity);
	TextItemRarity->SetColorAndOpacity(Color);
	ImageRarityBlock->SetColorAndOpacity(Color.GetSpecifiedColor());
}
