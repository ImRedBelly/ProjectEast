#include "InventoryUtility.h"
#include "Engine/DataTable.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractionComponent.h"
#include "ProjectEast/Core/Data/FGamepadButtonData.h"


UDataTable* InventoryUtility::GamepadIconsTable = nullptr;
TArray<FItemData*> InventoryUtility::CommonSortingItems;
TArray<FItemData*> InventoryUtility::SuperiorSortingItems;
TArray<FItemData*> InventoryUtility::EpicSortingItems;
TArray<FItemData*> InventoryUtility::LegendarySortingItems;
TArray<FItemData*> InventoryUtility::ConsumableSortingItems;
TArray<FItemData*> InventoryUtility::UsableSortingItems;
TArray<FItemData*> InventoryUtility::ArmorSortingItems;
TArray<FItemData*> InventoryUtility::WeaponSortingItems;
TArray<FItemData*> InventoryUtility::ShieldSortingItems;
TArray<FItemData*> InventoryUtility::FoodSortingItems;
TArray<FItemData*> InventoryUtility::PotionSortingItems;
TArray<FItemData*> InventoryUtility::CraftingIngredientSortingItems;
TArray<FItemData*> InventoryUtility::QuestItemSortingItems;
TArray<FItemData*> InventoryUtility::OtherSortingItems;
TArray<FItemData*> InventoryUtility::CurrencySortingItems;
TArray<FItemData*> InventoryUtility::CraftingRecipeSortingItems;


void InventoryUtility::Initialize(UDataTable* GamepadIcons)
{
	GamepadIconsTable = GamepadIcons;
}

UInteractableComponent* InventoryUtility::GetCurrentInteractableObject(AActor* OwningPlayer)
{
	UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(
		OwningPlayer->GetComponentByClass(UInteractionComponent::StaticClass()));

	if (IsValid(InteractionComponent) && IsValid(InteractionComponent->GetInteractableComponent()))
	{
		return InteractionComponent->GetInteractableComponent();
	}

	return nullptr;
}

UTexture2D* InventoryUtility::GetGamepadIcon(EGamepadButtonType GamepadInputs)
{
	for (FName RowName : GamepadIconsTable->GetRowNames())
	{
		auto DataTableRowInfo = GamepadIconsTable->FindRow<FGamepadButtonData>(RowName, TEXT(""));
		if (DataTableRowInfo->InputType == GamepadInputs)
			return DataTableRowInfo->InputIcon;
	}

	return GamepadIconsTable->FindRow<FGamepadButtonData>(GamepadIconsTable->GetRowNames()[0], TEXT(""))->InputIcon;
}

UPlayerInventory* InventoryUtility::GetPlayerInventory(AActor* OwningPlayer)
{
	if (IsValid(OwningPlayer))
	{
		UPlayerInventory* Inventory = Cast<UPlayerInventory>(
			OwningPlayer->GetComponentByClass(UPlayerInventory::StaticClass()));
		if (IsValid(Inventory))
			return Inventory;
	}
	return nullptr;
}

TTuple<bool, UInventoryCore*> InventoryUtility::GetInventoryFromInteractable(
	UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent))
	{
		UInventoryCore* Inventory = Cast<UInventoryCore>(
			InteractableComponent->GetOwner()->GetComponentByClass(UInventoryCore::StaticClass()));

		if (IsValid(Inventory))
			return MakeTuple(true, Inventory);
	}

	return MakeTuple(false, nullptr);
}

bool InventoryUtility::SwitchHasOwnerAuthority(UActorComponent* ActorComponent)
{
	return ActorComponent->GetOwner()->HasAuthority();
}

bool InventoryUtility::IsItemClassValid(const FItemData* ItemData)
{
	return IsValid(ItemData->Class);
}

bool InventoryUtility::AreItemsTheSame(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	return ItemDataFirst->ID == ItemDataSecond->ID;
}

bool InventoryUtility::AreItemsStackable(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	return ItemDataFirst->Class.GetDefaultObject()->bIsStackable && ItemDataSecond->Class.GetDefaultObject()->bIsStackable;
}

bool InventoryUtility::AreItemSlotsEqual(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	auto ResultFirst =(ItemDataFirst->ItemSlot == EItemSlot::Pocket1)
		|| (ItemDataFirst->ItemSlot == EItemSlot::Pocket2)
		|| (ItemDataFirst->ItemSlot == EItemSlot::Pocket3)
		|| (ItemDataFirst->ItemSlot == EItemSlot::Pocket4);
		auto ResultSecond =(ItemDataSecond->ItemSlot == EItemSlot::Pocket1)
		|| (ItemDataSecond->ItemSlot == EItemSlot::Pocket2)
		|| (ItemDataSecond->ItemSlot == EItemSlot::Pocket3)
		|| (ItemDataSecond->ItemSlot == EItemSlot::Pocket4);
	
	return ItemDataFirst->ItemSlot == ItemDataSecond->ItemSlot || (ResultFirst && ResultSecond);
}

bool InventoryUtility::CanWeaponsBeSwapped(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	if (IsItemClassValid(ItemDataSecond))
		if (ItemDataFirst->Class.GetDefaultObject()->WeaponType == EWeaponType::OneHand &&
			ItemDataSecond->Class.GetDefaultObject()->WeaponType == EWeaponType::OneHand)
			return true;
	
	return ItemDataFirst->Class.GetDefaultObject()->WeaponType == EWeaponType::OneHand &&
	(ItemDataSecond->Class.GetDefaultObject()->Type == EItemsType::Weapon ||
		ItemDataSecond->Class.GetDefaultObject()->Type == EItemsType::Shield);
}

bool InventoryUtility::IsStackableAndHaveStacks(const FItemData* ItemData, int32 Quantity)
{
	return ItemData->Class.GetDefaultObject()->bIsStackable && ItemData->Quantity > Quantity;
}

EInventoryPanels InventoryUtility::GetInventoryPanelFromItem(const FItemData* ItemData)
{
	return ItemData->Class.GetDefaultObject()->InventoryPanel;
}

TTuple<bool, int32> InventoryUtility::FindEmptySlotInArray(const TArray<FItemData*> ItemData)
{
	bool IsFindEmptyElement = false;
	int32 IndexEmptyElement = 0;

	for (int i = 0; i < ItemData.Num(); ++i)
	{
		auto Element = ItemData[i];
		if (!IsItemClassValid(Element))
		{
			IsFindEmptyElement = true;
			IndexEmptyElement = i;
			break;
		}
	}

	return MakeTuple(IsFindEmptyElement, IndexEmptyElement);
}

float InventoryUtility::CalculateStackedItemWeight(FItemData* ItemData)
{
	return ItemData->Class.GetDefaultObject()->Weight * FMathf::Clamp(ItemData->Quantity, 1, ItemData->Quantity);
}

void InventoryUtility::TakeAllItemsFromInventory(AActor* Interactor, UInventoryCore* ActorInventory)
{
	if (IsValid(ActorInventory))
	{
		auto PlayerInventory = GetPlayerInventory(Interactor);
		if (IsValid(PlayerInventory))
			PlayerInventory->ServerTakeAllItems(ActorInventory, Interactor);
	}
}

TArray<FItemData*> InventoryUtility::QuickSortItems(TArray<FItemData*> ItemData)
{
	TArray<FItemData*> SortedItems;
	for (int i = 0; i < ItemData.Num(); ++i)
	{
		if (IsItemClassValid(ItemData[i]))
		{
			SortedItems.Add(ItemData[i]);
		}
	}
	return SortedItems;
}

TArray<FItemData*> InventoryUtility::SortItemsByType(TArray<FItemData*> ItemData)
{
	TArray<FItemData*> SortedItems;
	for (int i = 0; i < ItemData.Num(); ++i)
	{
		EItemsType Type = ItemData[i]->Class.GetDefaultObject()->Type;

		if (!IsItemClassValid(ItemData[i]))
			continue;

		switch (Type)
		{
		case EItemsType::Armor:
			ArmorSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::Weapon:
			WeaponSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::Shield:
			ShieldSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::Potion:
			PotionSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::Food:
			FoodSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::CraftingIngredient:
			CraftingIngredientSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::CraftingRecipe:
			CraftingRecipeSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::QuestItem:
			QuestItemSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::Other:
			OtherSortingItems.Add(ItemData[i]);
			break;
		case EItemsType::Currency:
			CurrencySortingItems.Add(ItemData[i]);
			break;
		default: ;
		}
	}

	SortedItems.Append(ArmorSortingItems);
	SortedItems.Append(WeaponSortingItems);
	SortedItems.Append(ShieldSortingItems);
	SortedItems.Append(FoodSortingItems);
	SortedItems.Append(PotionSortingItems);
	SortedItems.Append(CraftingIngredientSortingItems);
	SortedItems.Append(QuestItemSortingItems);
	SortedItems.Append(OtherSortingItems);
	SortedItems.Append(CurrencySortingItems);
	SortedItems.Append(CraftingRecipeSortingItems);

	return SortedItems;
}

TArray<FItemData*> InventoryUtility::SortItemsByRarity(TArray<FItemData*> ItemData)
{
	TArray<FItemData*> SortedItems;
	for (int i = 0; i < ItemData.Num(); ++i)
	{
		EItemRarity Rarity = ItemData[i]->Class.GetDefaultObject()->Rarity;

		if (!IsItemClassValid(ItemData[i]))
			continue;

		switch (Rarity)
		{
		case EItemRarity::Common:
			CommonSortingItems.Add(ItemData[i]);
			break;
		case EItemRarity::Superior:
			SuperiorSortingItems.Add(ItemData[i]);
			break;
		case EItemRarity::Epic:
			EpicSortingItems.Add(ItemData[i]);
			break;
		case EItemRarity::Legendary:
			LegendarySortingItems.Add(ItemData[i]);
			break;
		case EItemRarity::Consumable:
			ConsumableSortingItems.Add(ItemData[i]);
			break;
		case EItemRarity::Usable:
			UsableSortingItems.Add(ItemData[i]);
			break;
		default: ;
		}
	}

	SortedItems.Append(CommonSortingItems);
	SortedItems.Append(SuperiorSortingItems);
	SortedItems.Append(EpicSortingItems);
	SortedItems.Append(LegendarySortingItems);
	SortedItems.Append(ConsumableSortingItems);
	SortedItems.Append(UsableSortingItems);

	return SortedItems;
}

TArray<FItemData*> InventoryUtility::SortItemsByValue(TArray<FItemData*> ItemData)
{
	bool IsFound = false;
	TArray<FItemData*> SortedItems;
	for (int i = 0; i < ItemData.Num(); ++i)
	{
		if (IsItemClassValid(ItemData[i]))
		{
			float NormalValue = ItemData[i]->Class.GetDefaultObject()->Value;

			for (int a = 0; a < SortedItems.Num(); ++a)
			{
				if (IsItemClassValid(SortedItems[a]))
				{
					float SortedValue = SortedItems[a]->Class.GetDefaultObject()->Value;
					if (NormalValue > SortedValue)
					{
						IsFound = true;
						SortedItems.Insert(ItemData[i], a);
						break;
					}
				}
			}


			if (IsFound)
				IsFound = false;
			else
				SortedItems.Add(ItemData[i]);
		}
	}


	return SortedItems;
}

TArray<FItemData*> InventoryUtility::SortItemsByWeight(TArray<FItemData*> ItemData)
{
	bool IsFound = false;
	TArray<FItemData*> SortedItems;
	for (int i = 0; i < ItemData.Num(); ++i)
	{
		if (IsItemClassValid(ItemData[i]))
		{
			float NormalWeight = ItemData[i]->Class.GetDefaultObject()->Weight;

			for (int a = 0; a < SortedItems.Num(); ++a)
			{
				if (IsItemClassValid(SortedItems[a]))
				{
					float SortedWeight = SortedItems[a]->Class.GetDefaultObject()->Weight;
					if (NormalWeight > SortedWeight)
					{
						IsFound = true;
						SortedItems.Insert(ItemData[i], a);
						break;
					}
				}
			}


			if (IsFound)
				IsFound = false;
			else
				SortedItems.Add(ItemData[i]);
		}
	}

	return SortedItems;
}

FSlateColor InventoryUtility::GetRarityColor(EItemRarity ItemRarity)
{
	switch (ItemRarity)
	{
	case EItemRarity::None:
		return FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f));
	case EItemRarity::Common:
		return FSlateColor(FLinearColor(0.775822f, 0.775822f, 0.775822f));
	case EItemRarity::Superior:
		return FSlateColor(FLinearColor(0.0f, 0.411458f, 0.155397f));
	case EItemRarity::Epic:
		return FSlateColor(FLinearColor(0.387001f, 0.0f, 0.697917f));
	case EItemRarity::Legendary:
		return FSlateColor(FLinearColor(0.645833f, 0.477914f, 0.045227f));
	case EItemRarity::Consumable:
		return FSlateColor(FLinearColor(0.05448f, 0.291771f, 0.53948f));
	case EItemRarity::Usable:
		return FSlateColor(FLinearColor(0.6875f, 0.224721f, 0.072573f));
	}
	return FSlateColor(FLinearColor(0.775822f, 0.775822f, 0.775822f));
}

void InventoryUtility::PlaySoundOnOpeningWidget()
{
}

void InventoryUtility::PlaySoundOnItemDropped()
{
}

void InventoryUtility::PlaySoundOnTabSwitched()
{
}
