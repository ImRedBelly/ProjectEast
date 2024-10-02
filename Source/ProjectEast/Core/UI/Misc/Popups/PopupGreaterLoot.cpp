#include "PopupGreaterLoot.h"

#include "Kismet/KismetTextLibrary.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UPopupGreaterLoot::InitializePopup(UPlayerInventory* InPlayerInventory)
{
	PlayerInventory = InPlayerInventory;

	if (IsValid(PlayerInventory))
	{
		PlayerInventory->OnItemLooted.AddDynamic(this, &UPopupGreaterLoot::OnItemLooted);
	}
}

void UPopupGreaterLoot::NativeDestruct()
{
	if (IsValid(PlayerInventory))
		PlayerInventory->OnItemLooted.RemoveDynamic(this, &UPopupGreaterLoot::OnItemLooted);
	Super::NativeDestruct();
}

void UPopupGreaterLoot::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	SetVisibility(ESlateVisibility::Collapsed);
	DisplayNewItem();
	Super::OnAnimationFinished_Implementation(Animation);
}

void UPopupGreaterLoot::OnItemLooted(FItemData& ItemData)
{
	FItemData* Data = &ItemData;
	if (IsEligibleToDisplay(Data))
	{
		LootedItems.Add(Data);
		DisplayNewItem();
	}
}

void UPopupGreaterLoot::DisplayNewItem()
{
	for (int i = LootedItems.Num() - 1; i >= 0; --i)
	{
		if (IsAnimationPlaying(ConstructAnimation))
			break;

		SetViewItemData(LootedItems[i]);
		SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(ConstructAnimation);

		LootedItems.RemoveAt(i);
	}
}

void UPopupGreaterLoot::SetViewItemData(FItemData* ItemData)
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		ImageItem->SetBrushFromTexture(ItemData->Class.GetDefaultObject()->ImageItem);
		TextItemName->SetText(ItemData->Class.GetDefaultObject()->NameItem);
		TextItemMessage->SetText(GetTextMessageBasedOnItem(ItemData));
		TextItemQuantity->SetText(FText::FromString("x" + FString::FromInt(ItemData->Quantity)));
	}
}

bool UPopupGreaterLoot::IsEligibleToDisplay(FItemData* ItemData)
{
	return !UKismetTextLibrary::EqualEqual_TextText(GetTextMessageBasedOnItem(ItemData), FText());
}

FText UPopupGreaterLoot::GetTextMessageBasedOnItem(FItemData* ItemData)
{
	if (InventoryUtility::IsItemClassValid(ItemData))
		return ItemData->Class.GetDefaultObject()->GreatLootText;
	return FText();
}
