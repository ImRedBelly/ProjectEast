#pragma once
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Data/FGamepadButtonData.h"

class UInteractableComponent;

class InventoryUtility
{
public:
	static void Initialize(UDataTable* GamepadIcons);

	static UInteractableComponent* GetCurrentInteractableObject(AActor* OwningPlayer);
	static UTexture2D* GetGamepadIcon(EGamepadButtonType GamepadInputs);
	static UPlayerInventory* GetPlayerInventory(AActor* OwningPlayer);
	static TTuple<bool, UInventoryCore*> GetInventoryFromInteractable(UInteractableComponent* InteractableComponent);
	static bool SwitchHasOwnerAuthority(UActorComponent* ActorComponent);
	static bool IsItemClassValid(const FItemData* ItemData);
	static bool AreItemsTheSame(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool AreItemsStackable(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool AreItemSlotsEqual(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool CanWeaponsBeSwapped(const FItemData* ItemDataFirst, const FItemData* ItemDataSecond);
	static bool IsStackableAndHaveStacks(const FItemData* ItemData, int32 Quantity);
	static EInventoryPanels GetInventoryPanelFromItem(const FItemData* ItemData);
	static TTuple<bool, int32> FindEmptySlotInArray(const TArray<FItemData*> ItemData);
	static float CalculateStackedItemWeight(FItemData* ItemData);
	static void TakeAllItemsFromInventory(AActor* Interactor, UInventoryCore* ActorInventory);;

	static TArray<FItemData*> QuickSortItems(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByType(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByRarity(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByValue(TArray<FItemData*> ItemData);
	static TArray<FItemData*> SortItemsByWeight(TArray<FItemData*> ItemData);
	static FSlateColor GetRarityColor(EItemRarity ItemRarity);
	static void PlaySoundOnOpeningWidget();
	static void PlaySoundOnItemDropped();

	static UDataTable* GamepadIconsTable;

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
