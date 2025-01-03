﻿#pragma once
#include "ProjectEast/Core/Utils/GameTypes.h"

class UInventoryCore;
class UCraftingCore;
class UConsumableBuffs;
class UPlayerCrafting;
class UPlayerEquipment;
class UPlayerInventory;
class UPlayerLeveling;
class UInteractableComponent;

class InventoryUtility
{
public:
	static UInteractableComponent* GetCurrentInteractableObject(AActor* OwningPlayer);
	static UPlayerInventory* GetPlayerInventory(AActor* OwningPlayer);
	static UPlayerEquipment* GetPlayerEquipment(AActor* OwningPlayer);
	static UPlayerCrafting* GetPlayerCrafting(AActor* OwningPlayer);
	static UConsumableBuffs* GetConsumableBuffs(AActor* OwningPlayer);
	static UPlayerLeveling* GetPlayerLeveling(AActor* OwningPlayer);
	static UCraftingCore* GetCurrentCraftingStation(AActor* OwningPlayer);
	static TTuple<bool, UInventoryCore*> GetInventoryFromInteractable(UInteractableComponent* InteractableComponent);
	static bool SwitchHasOwnerAuthority(UActorComponent* ActorComponent);
	static bool IsItemClassValid(const FItemData* ItemData);
	static bool IsCraftingDataValid(const FCraftingData* CraftingData);
	static bool AreItemsTheSame(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool AreItemsStackable(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool AreItemSlotsEqual(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool AreWeaponTypesEqual(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool CanWeaponsBeSwapped(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool IsStackableAndHaveStacks(const FItemData* ItemData, int32 Quantity);
	static bool AreCraftingDatasTheSame(const FCraftingData& CraftingData, FCraftingData* StoredCraftingData);
	static TTuple<bool, int32> HasPartialStack(const TArray<FItemData*> ItemDataArray, FItemData* ItemData);
	static EInventoryPanels GetInventoryPanelFromItem(const FItemData* ItemData);
	static TTuple<bool, int32> FindEmptySlotInArray(const TArray<FItemData*> ItemData);
	static TTuple<bool, FItemData*> FindItemByID(const TArray<FItemData*> ItemData, FString ID);
	static TTuple<bool, int32> FindItemIndex(const TArray<FItemData*> ItemDataArray, FItemData* ItemData);
	static TTuple<bool, EItemSlot> FindEmptyEquipmentSlot(TMap<EItemSlot, FItemData*> EquipmentData, TArray<EItemSlot> SlotsToSearch, EItemSlot DefaultSlot);
	static float CalculateStackedItemWeight(FItemData* ItemData);
	static float CalculateStackedItemValue(FItemData* ItemData);
	static void TakeAllItemsFromInventory(AActor* Interactor, UInventoryCore* ActorInventory);
	static bool CanItemBeRemoved(FItemData* ItemData);
	static TArray<FItemData*> GetAllItemsOfType(TArray<FItemData*> ItemData, EItemsType ItemsType);
	static int32 FindAmountOfEmptySlots(TArray<FItemData*> ItemData);
	static TTuple<TArray<FItemData*>, TArray<FSingleDTItem>> GetCraftableData(FCraftingData* CraftingData);

	static FItemData* CopyItemData(FItemData* ItemData);
	static FItemData* CopyItemData(FItemData ItemData);
	static FCraftingData* CopyCraftingData(FCraftingData* ItemData);
	
	static TArray<FItemData*> QuickSortItems(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByType(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByRarity(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByValue(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByWeight(TArray<FItemData*> ItemData);
	static FSlateColor GetRarityColor(EItemRarity ItemRarity);
	static void PlaySoundOnOpeningWidget();
	static void PlaySoundOnItemDropped();
	static void PlaySoundOnTabSwitched();
	static void PlaySoundOnItemPickedUp();
	static FCraftingData* GetCraftingDataFromTableRow(const FDataTableRowHandle& TableRow);

	static TArray<FItemData*> CommonSortingItems;
	static TArray<FItemData*> SuperiorSortingItems;
	static TArray<FItemData*> EpicSortingItems;
	static TArray<FItemData*> LegendarySortingItems;
	static TArray<FItemData*> ConsumableSortingItems;
	static TArray<FItemData*> UsableSortingItems;


	static TArray<FItemData*> ArmorSortingItems;
	static TArray<FItemData*> WeaponSortingItems;
	static TArray<FItemData*> ShieldSortingItems;
	static TArray<FItemData*> FoodSortingItems;
	static TArray<FItemData*> PotionSortingItems;
	static TArray<FItemData*> CraftingIngredientSortingItems;
	static TArray<FItemData*> QuestItemSortingItems;
	static TArray<FItemData*> OtherSortingItems;
	static TArray<FItemData*> CurrencySortingItems;
	static TArray<FItemData*> CraftingRecipeSortingItems;
};
