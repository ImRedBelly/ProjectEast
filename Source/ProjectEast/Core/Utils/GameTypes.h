#pragma once
#include "GameTypes.generated.h"

#define ECC_Interactable ECC_GameTraceChannel1
#define ECC_Climbable ECC_GameTraceChannel2

class UMainItemData;
const FName ProjectEast = FName("ProjectEast");
const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");


const FName SocketFPCamera = FName("FPCamera");

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");


UENUM(BlueprintType)
enum class EMovementState : uint8
{
	None UMETA(DisplayName = "None"),
	Grounded UMETA(DisplayName = "Grounded"),
	InAir UMETA(DisplayName = "InAir"),
	Mantling UMETA(DisplayName = "Mantling"),
	Ragdoll UMETA(DisplayName = "Ragdoll"),
};
UENUM(BlueprintType)
enum class EMovementAction : uint8
{
	None UMETA(DisplayName = "None"),
	LowMantle UMETA(DisplayName = "LowMantle"),
	HighMantle UMETA(DisplayName = "HighMantle"),
	Rolling UMETA(DisplayName = "Rolling"),
	GettingUp UMETA(DisplayName = "GettingUp"),
};


UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Sprinting UMETA(DisplayName = "Sprinting"),
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	VelocityDirection UMETA(DisplayName = "VelocityDirection"),
	LookingDirection UMETA(DisplayName = "LookingDirection"),
	Aiming UMETA(DisplayName = "Aiming"),
};

UENUM(BlueprintType)
enum class EViewMode : uint8
{
	ThirdPerson UMETA(DisplayName = "ThirdPerson"),
	FirstPerson UMETA(DisplayName = "FirstPerson"),
};


UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	Default UMETA(DisplayName = "Default"),
	Masculine UMETA(DisplayName = "Masculine"),
	Feminine UMETA(DisplayName = "Feminine"),
	Injured UMETA(DisplayName = "Injured"),
	HandsTied UMETA(DisplayName = "HandsTied"),
	Rifle UMETA(DisplayName = "Rifle"),
	Pistol1H UMETA(DisplayName = "Pistol 1H"),
	Pistol2H UMETA(DisplayName = "Pistol 2H"),
	Bow UMETA(DisplayName = "Bow"),
	Torch UMETA(DisplayName = "Torch"),
	Binoculars UMETA(DisplayName = "Binoculars"),
	Box UMETA(DisplayName = "Box"),
	Barrel UMETA(DisplayName = "Barrel"),
};


UENUM(BlueprintType)
enum class EStance : uint8
{
	Standing UMETA(DisplayName = "Standing"),
	Crouching UMETA(DisplayName = "Crouching"),
};


UENUM(BlueprintType)
enum class EMantleType : uint8
{
	HighMantle UMETA(DisplayName = "HighMantle"),
	LowMantle UMETA(DisplayName = "LowMantle"),
	FallingCatch UMETA(DisplayName = "FallingCatch"),
};

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
enum class EItemDestination : uint8
{
	InventorySlot UMETA(DisplayName = "InventorySlot"),
	EquipmentSlot UMETA(DisplayName = "EquipmentSlot"),
	VendorSlot UMETA(DisplayName = "VendorSlot"),
	StorageSlot UMETA(DisplayName = "StorageSlot"),
	DropBar UMETA(DisplayName = "DropBar"),
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
enum class EWeaponSubtype : uint8
{
	None UMETA(DisplayName = "None"),
	Sword UMETA(DisplayName = "Sword"),
	Warhammer UMETA(DisplayName = "Warhammer"),
	Axe UMETA(DisplayName = "Axe"),
	Mace UMETA(DisplayName = "Mace"),
};

UENUM(BlueprintType)
enum class EWidgetType : uint8
{
	None UMETA(DisplayName = "None"),
	Pause UMETA(DisplayName = "Pause"),
	Inventory UMETA(DisplayName = "Inventory"),
	Equipment UMETA(DisplayName = "Equipment"),
	PlayerCrafting UMETA(DisplayName = "PlayerCrafting"),
	StationCrafting UMETA(DisplayName = "StationCrafting"),
	Vendor UMETA(DisplayName = "Vendor"),
	Storage UMETA(DisplayName = "Storage"),
	LoadGame UMETA(DisplayName = "LoadGame"),
	Abilities UMETA(DisplayName = "Abilities"),
	Map UMETA(DisplayName = "Map"),
	Quests UMETA(DisplayName = "Quests"),
};

UENUM(BlueprintType)
enum class EWidgetPopup : uint8
{
	None UMETA(DisplayName = "None"),
	SplitStackPopup UMETA(DisplayName = "SplitStackPopup"),
	ConfirmationPopup UMETA(DisplayName = "ConfirmationPopup"),
	TextDocumentPopup UMETA(DisplayName = "TextDocumentPopup"),
};

UENUM(BlueprintType)
enum class EStateCategoryType : uint8
{
	None UMETA(DisplayName = "None"),
	Health UMETA(DisplayName = "Health"),
	MaxHealth UMETA(DisplayName = "MaxHealth"),
	Stamina UMETA(DisplayName = "Stamina"),
	MaxStamina UMETA(DisplayName = "MaxStamina"),
	Armor UMETA(DisplayName = "Armor"),
	Damage UMETA(DisplayName = "Damage"),
	AttackSpeed UMETA(DisplayName = "AttackSpeed"),
};

UENUM(BlueprintType)
enum class EBuffEffectCategory : uint8
{
	None UMETA(DisplayName = "None"),
	AddInstantly UMETA(DisplayName = "AddInstantly"),
	AddOverDuration UMETA(DisplayName = "AddOverDuration"),
	AddForDuration UMETA(DisplayName = "AddForDuration"),
};

UENUM(BlueprintType)
enum class EAdditionalBuffEffect : uint8
{
	None UMETA(DisplayName = "None"),
	Drunk UMETA(DisplayName = "Drunk"),
	NightVision UMETA(DisplayName = "NightVision"),
};

UENUM(BlueprintType)
enum class EStatCategory : uint8
{
	None UMETA(DisplayName = "None"),
	Health UMETA(DisplayName = "Health"),
	MaxHealth UMETA(DisplayName = "MaxHealth"),
	Stamina UMETA(DisplayName = "Stamina"),
	MaxStamina UMETA(DisplayName = "MaxStamina"),
	Armor UMETA(DisplayName = "Armor"),
	Damage UMETA(DisplayName = "Damage"),
	AttackSpeed UMETA(DisplayName = "AttackSpeed"),
};

UENUM(BlueprintType)
enum class EInteractionInputType : uint8
{
	Single UMETA(DisplayName = "Single"),
	Holding UMETA(DisplayName = "Holding"),
	Multiple UMETA(DisplayName = "Multiple/Mashing"),
};

UENUM(BlueprintType)
enum class EInteractionResponse : uint8
{
	Persistent UMETA(DisplayName = "Persistent"),
	OnlyOnce UMETA(DisplayName = "OnlyOnce"),
	Temporary UMETA(DisplayName = "Temporary"),
};


UENUM(BlueprintType)
enum class ECraftingCategory : uint8
{
	None UMETA(DisplayName = "None"),
	Armor UMETA(DisplayName = "Armor"),
	Weapons UMETA(DisplayName = "Weapons"),
};

UENUM(BlueprintType)
enum class ECraftingSubcategory : uint8
{
	None UMETA(DisplayName = "None"),
	HeadArmor UMETA(DisplayName = "HeadArmor"),
	Chestplates UMETA(DisplayName = "Chestplates"),
	Gloves UMETA(DisplayName = "Gloves"),
	Necklaces UMETA(DisplayName = "Necklaces"),
	Legs UMETA(DisplayName = "Legs"),
	Boots UMETA(DisplayName = "Boots"),
	Swords UMETA(DisplayName = "Swords"),
	Hammers UMETA(DisplayName = "Hammers"),
	Axes UMETA(DisplayName = "Axes"),
	Shields UMETA(DisplayName = "Shields"),
};

UENUM(BlueprintType)
enum class ECraftingStation : uint8
{
	None UMETA(DisplayName = "None"),
	Forge UMETA(DisplayName = "Forge"),
};


UENUM(BlueprintType)
enum class EGroundedEntryState : uint8
{
	None UMETA(DisplayName = "None"),
	Roll UMETA(DisplayName = "Roll"),
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward UMETA(DisplayName = "Forward"),
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
	Backward UMETA(DisplayName = "Backward"),
};

UENUM(BlueprintType)
enum class ETrackedHipsDirection : uint8
{
	F UMETA(DisplayName = "F"),
	B UMETA(DisplayName = "B"),
	RF UMETA(DisplayName = "RF"),
	RB UMETA(DisplayName = "RB"),
	LF UMETA(DisplayName = "LF"),
	LB UMETA(DisplayName = "LB"),
};

USTRUCT(BlueprintType)
struct FSingleDTItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle TableAndRow;

	UPROPERTY(EditAnywhere)
	int32 Quantity;
};


USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FString ID;
	UPROPERTY(EditDefaultsOnly)
	EItemSlot EquipmentSlot;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainItemData> Class;
	UPROPERTY(EditDefaultsOnly)
	int32 Quantity;
	UPROPERTY(EditDefaultsOnly)
	int32 Durability;
	UPROPERTY(EditDefaultsOnly)
	int32 Index;
	UPROPERTY(EditDefaultsOnly)
	bool bIsEquipped;
	UPROPERTY(EditDefaultsOnly)
	bool bIsAlreadyUsed;
	UPROPERTY(EditDefaultsOnly)
	float ValueModifier;
};


USTRUCT(BlueprintType)
struct FCraftingData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FString CraftingID;
	UPROPERTY(EditDefaultsOnly)
	TArray<FSingleDTItem> OutputItems;
	UPROPERTY(EditDefaultsOnly)
	TArray<FSingleDTItem> InputItems;
	UPROPERTY(EditDefaultsOnly)
	bool StartLocked;
	UPROPERTY(EditDefaultsOnly)
	float CraftingTime;
	UPROPERTY(EditDefaultsOnly)
	float CraftingCost;
	UPROPERTY(EditDefaultsOnly)
	ECraftingCategory Category;
	UPROPERTY(EditDefaultsOnly)
	ECraftingSubcategory Subcategory;
	UPROPERTY(EditDefaultsOnly)
	TArray<ECraftingStation> RequiredStations;
	UPROPERTY(EditDefaultsOnly)
	int32 CraftingCounter;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxCount;
};


USTRUCT(BlueprintType)
struct FRandomizedLootTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere)
	int32 MinLootItems;

	UPROPERTY(EditAnywhere)
	int32 MaxLootItems;
};

USTRUCT()
struct FItemsStatRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EStateCategoryType Type;

	UPROPERTY(EditAnywhere)
	float Value;
};
