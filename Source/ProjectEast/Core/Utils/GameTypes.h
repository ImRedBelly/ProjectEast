#pragma once

#define ECC_Interactable ECC_GameTraceChannel1


const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");


const FName DebugCategoryLedgeDetection = FName("LedgeDetection");


UENUM(BlueprintType)
enum class EItemSlot : uint8
{
	None UMETA(DisplayName = "None"),
	Head UMETA(DisplayName = "Head"),
	Gloves UMETA(DisplayName = "Gloves"),
	Neck UMETA(DisplayName = "Neck"),
	Chest UMETA(DisplayName = "Chest"),
	Legs UMETA(DisplayName = "Legs"),
	Boots UMETA(DisplayName = "Boots"),
	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Pocket1 UMETA(DisplayName = "Pocket1"),
	Pocket2 UMETA(DisplayName = "Pocket2"),
	Pocket3 UMETA(DisplayName = "Pocket3"),
	Pocket4 UMETA(DisplayName = "Pocket4"),
};

UENUM(BlueprintType)
enum class EInventoryPanels : uint8
{
	None UMETA(DisplayName = "None"),
	P1 UMETA(DisplayName = "P1"),
	P2 UMETA(DisplayName = "P2"),
	P3 UMETA(DisplayName = "P3"),
	P4 UMETA(DisplayName = "P4"),
};

UENUM(BlueprintType)
enum class EInputMethodType : uint8
{
	RightClick UMETA(DisplayName = "RightClick"),
	DragAndDrop UMETA(DisplayName = "DragAndDrop"),
};

UENUM(BlueprintType)
enum class ESortMethod : uint8
{
	Quicksort UMETA(DisplayName = "Quicksort"),
	ByType UMETA(DisplayName = "ByType"),
	ByRarity UMETA(DisplayName = "ByRarity"),
	ByValue UMETA(DisplayName = "ByValue"),
	ByWeight UMETA(DisplayName = "ByWeight"),
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	None UMETA(DisplayName = "None"),
	Common UMETA(DisplayName = "Common"),
	Superior UMETA(DisplayName = "Superior"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary"),
	Consumable UMETA(DisplayName = "Consumable"),
	Usable UMETA(DisplayName = "Usable"),
};

UENUM(BlueprintType)
enum class EItemsType : uint8
{
	None UMETA(DisplayName = "None"),
	Armor UMETA(DisplayName = "Armor"),
	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Food UMETA(DisplayName = "Food"),
	Potion UMETA(DisplayName = "Potion"),
	CraftingIngredient UMETA(DisplayName = "CraftingIngredient"),
	QuestItem UMETA(DisplayName = "QuestItem"),
	Other UMETA(DisplayName = "Other"),
	Currency UMETA(DisplayName = "Currency"),
	CraftingRecipe UMETA(DisplayName = "CraftingRecipe"),
};

UENUM(BlueprintType)
enum class EItemRemoveType : uint8
{
	Default UMETA(DisplayName = "Default"),
	OnConfirmation UMETA(DisplayName = "OnConfirmation"),
	CannotBeRemoved UMETA(DisplayName = "CannotBeRemoved"),
};

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None UMETA(DisplayName = "None"),
	TextDocument UMETA(DisplayName = "TextDocument"),
	Currency UMETA(DisplayName = "Currency"),
	CraftingRecipe UMETA(DisplayName = "CraftingRecipe"),
};
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None UMETA(DisplayName = "None"),
	OneHand UMETA(DisplayName = "OneHand"),
	TwoHand UMETA(DisplayName = "TwoHand"),
	OffHand UMETA(DisplayName = "OffHand"),
};



UENUM(BlueprintType)
enum class EWidgetType : uint8
{
	None UMETA(DisplayName = "None"),
	Inventory UMETA(DisplayName = "Inventory"),
	Equipment UMETA(DisplayName = "Equipment"),
	Crafting UMETA(DisplayName = "Crafting"),
	Vendor UMETA(DisplayName = "Vendor"),
	Storage UMETA(DisplayName = "Storage"),
	LoadGame UMETA(DisplayName = "LoadGame"),
	Abilities UMETA(DisplayName = "Abilities"),
};
