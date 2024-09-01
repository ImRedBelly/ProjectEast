#include "InventoryUtility.h"
#include "Engine/DataTable.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractionComponent.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/Data/FGamepadButtonData.h"

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

UPlayerEquipment* InventoryUtility::GetPlayerEquipment(AActor* OwningPlayer)
{
	if (IsValid(OwningPlayer))
	{
		UPlayerEquipment* Equipment = Cast<UPlayerEquipment>(
			OwningPlayer->GetComponentByClass(UPlayerEquipment::StaticClass()));
		if (IsValid(Equipment))
			return Equipment;
	}
	return nullptr;
}

UConsumableBuffs* InventoryUtility::GetConsumableBuffs(AActor* OwningPlayer)
{
	if (IsValid(OwningPlayer))
	{
		UConsumableBuffs* ConsumableBuffs = Cast<UConsumableBuffs>(
			OwningPlayer->GetComponentByClass(UConsumableBuffs::StaticClass()));
		if (IsValid(ConsumableBuffs))
			return ConsumableBuffs;
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
	return ItemData->Class != nullptr || IsValid(ItemData->Class);
}

bool InventoryUtility::AreItemsTheSame(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	return ItemDataFirst->ID == ItemDataSecond->ID;
}

bool InventoryUtility::AreItemsStackable(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	return ItemDataFirst->Class.GetDefaultObject()->bIsStackable && ItemDataSecond->Class.GetDefaultObject()->
		bIsStackable;
}

bool InventoryUtility::AreItemSlotsEqual(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	auto ResultFirst = (ItemDataFirst->EquipmentSlot == EItemSlot::Pocket1)
		|| (ItemDataFirst->EquipmentSlot == EItemSlot::Pocket2)
		|| (ItemDataFirst->EquipmentSlot == EItemSlot::Pocket3)
		|| (ItemDataFirst->EquipmentSlot == EItemSlot::Pocket4);
	
	auto ResultSecond = (ItemDataSecond->EquipmentSlot == EItemSlot::Pocket1)
		|| (ItemDataSecond->EquipmentSlot == EItemSlot::Pocket2)
		|| (ItemDataSecond->EquipmentSlot == EItemSlot::Pocket3)
		|| (ItemDataSecond->EquipmentSlot == EItemSlot::Pocket4);

	return ItemDataFirst->EquipmentSlot == ItemDataSecond->EquipmentSlot || (ResultFirst && ResultSecond);
}

bool InventoryUtility::AreWeaponTypesEqual(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	if (IsItemClassValid(ItemDataSecond))
		return ItemDataFirst->Class.GetDefaultObject()->WeaponType == ItemDataSecond->Class.GetDefaultObject()->
			WeaponType;
	return false;
}

bool InventoryUtility::CanWeaponsBeSwapped(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond)
{
	if (IsItemClassValid(ItemDataSecond))
		if (ItemDataFirst->Class.GetDefaultObject()->WeaponType == EWeaponType::OneHand &&
			ItemDataSecond->Class.GetDefaultObject()->WeaponType == EWeaponType::OneHand)
			return true;

	return ItemDataFirst->Class.GetDefaultObject()->WeaponType == EWeaponType::OneHand &&
	(ItemDataSecond->EquipmentSlot == EItemSlot::Weapon ||
		ItemDataSecond->EquipmentSlot == EItemSlot::Shield);
}

bool InventoryUtility::IsStackableAndHaveStacks(const FItemData* ItemData, int32 Quantity)
{
	return ItemData->Class.GetDefaultObject()->bIsStackable && ItemData->Quantity > Quantity;
}

TTuple<bool, uint32> InventoryUtility::HasPartialStack(const TArray<FItemData*> ItemDataArray, FItemData* ItemData)
{
	for (int i = 0; i < ItemDataArray.Num(); ++i)
	{
		if (IsItemClassValid(ItemDataArray[i]))
		{
			if (ItemDataArray[i]->ID == ItemData->ID && ItemDataArray[i]->Class.GetDefaultObject()->bIsStackable &&
				ItemData->Class.GetDefaultObject()->bIsStackable)
				return MakeTuple(true, i);
		}
	}
	return MakeTuple(false, 0);
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

TTuple<bool, FItemData*> InventoryUtility::FindItemByID(const TArray<FItemData*> ItemData, FString ID)
{
	for (int i = 0; i < ItemData.Num(); ++i)
	{
		if (ItemData[i]->ID == ID)
			return MakeTuple(true, ItemData[i]);
	}
	return MakeTuple(true, ItemData[0]);
}

TTuple<bool, uint32> InventoryUtility::FindItemIndex(const TArray<FItemData*> ItemDataArray, FItemData* ItemData)
{
	for (int i = 0; i < ItemDataArray.Num(); ++i)
		if (AreItemsTheSame(ItemDataArray[i], ItemData))
			return MakeTuple(true, i);

	return MakeTuple(false, 0);
}

TTuple<bool, EItemSlot> InventoryUtility::FindEmptyEquipmentSlot(TMap<EItemSlot, FItemData*> EquipmentData,
                                                                 TArray<EItemSlot> SlotsToSearch, EItemSlot DefaultSlot)
{
	for (auto Slot : SlotsToSearch)
		if (EquipmentData.Contains(Slot))
			if (IsItemClassValid(EquipmentData[Slot]))
				return MakeTuple(true, Slot);

	return MakeTuple(false, DefaultSlot);
}

float InventoryUtility::CalculateStackedItemWeight(FItemData* ItemData)
{
	return ItemData->Class.GetDefaultObject()->Weight * FMathf::Clamp(ItemData->Quantity, 1, ItemData->Quantity);
}

float InventoryUtility::CalculateStackedItemValue(FItemData* ItemData)
{
	auto Quantity = FMathf::Clamp(ItemData->Quantity, 0, ItemData->Quantity);
	return Quantity * (ItemData->Class.GetDefaultObject()->Value * ItemData->ValueModifier);
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

bool InventoryUtility::CanItemBeRemoved(FItemData* ItemData)
{
	return ItemData->Class.GetDefaultObject()->RemoveType != EItemRemoveType::CannotBeRemoved;
}

TArray<FItemData*> InventoryUtility::GetAllItemsOfType(TArray<FItemData*> ItemData, EItemsType ItemsType)
{
	TArray<FItemData*> ItemsDataByType;
	for (int i = 0; i < ItemData.Num(); ++i)
		if (IsItemClassValid(ItemData[i]))
			if (ItemData[i]->Class.GetDefaultObject()->Type == ItemsType)
				ItemsDataByType.Add(ItemData[i]);

	return ItemsDataByType;
}

uint32 InventoryUtility::FindAmountOfEmptySlots(TArray<FItemData*> ItemData)
{
	uint32 Amount = 0;
	for (int i = 0; i < ItemData.Num(); ++i)
	{
		if (!IsItemClassValid(ItemData[i]))
			Amount++;
	}
	return Amount;
}

FItemData* InventoryUtility::CopyItemData(FItemData* ItemData)
{
	FItemData* NewItemData = new FItemData();
	NewItemData->ID = ItemData->ID;
	NewItemData->EquipmentSlot = ItemData->EquipmentSlot;
	NewItemData->Class = ItemData->Class;
	NewItemData->Quantity = ItemData->Quantity;
	NewItemData->Durability = ItemData->Durability;
	NewItemData->Index = ItemData->Index;
	NewItemData->bIsEquipped = ItemData->bIsEquipped;
	NewItemData->bIsAlreadyUsed = ItemData->bIsAlreadyUsed;
	NewItemData->ValueModifier = ItemData->ValueModifier;
	return NewItemData;
}

FItemData* InventoryUtility::CopyItemData(FItemData ItemData)
{
	FItemData* NewItemData = new FItemData();
	NewItemData->ID = ItemData.ID;
	NewItemData->EquipmentSlot = ItemData.EquipmentSlot;
	NewItemData->Class = ItemData.Class;
	NewItemData->Quantity = ItemData.Quantity;
	NewItemData->Durability = ItemData.Durability;
	NewItemData->Index = ItemData.Index;
	NewItemData->bIsEquipped = ItemData.bIsEquipped;
	NewItemData->bIsAlreadyUsed = ItemData.bIsAlreadyUsed;
	NewItemData->ValueModifier = ItemData.ValueModifier;
	return NewItemData;
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
		if (!IsItemClassValid(ItemData[i]))
			continue;

		EItemsType Type = ItemData[i]->Class.GetDefaultObject()->Type;

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
		if (!IsItemClassValid(ItemData[i]))
			continue;

		EItemRarity Rarity = ItemData[i]->Class.GetDefaultObject()->Rarity;

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

void InventoryUtility::PlaySoundOnItemPickedUp()
{
}
