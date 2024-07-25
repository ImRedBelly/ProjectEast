#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "MainItemData.generated.h"

class ABaseItemPickUp;

USTRUCT()
struct FItemsStatRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EStateCategoryType Type;

	UPROPERTY(EditAnywhere)
	float Value;
};


UCLASS()
class PROJECTEAST_API UMainItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Item Info")
	FText NameItem;
	UPROPERTY(EditDefaultsOnly, Category="Item Info")
	FText Description;
	UPROPERTY(EditDefaultsOnly, Category="Item Info")
	FText GreatLootText;

	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	UTexture2D* ImageItem;
	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	EItemsType Type;
	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	EItemRarity Rarity;
	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	EInventoryPanels InventoryPanel = EInventoryPanels::P1;
	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	EWeaponType WeaponType;
	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	EWeaponSubtype WeaponSubtype;

	UPROPERTY(EditDefaultsOnly, Category="Item Stats")
	float Weight;
	UPROPERTY(EditDefaultsOnly, Category="Item Stats")
	float Value;
	UPROPERTY(EditDefaultsOnly, Category="Item Stats")
	uint32 RequiredLevel;
	UPROPERTY(EditDefaultsOnly, Category="Item Stats")
	TArray<FItemsStatRow> Stats;

	UPROPERTY(EditDefaultsOnly, Category="Item Stacking")
	bool bIsStackable;

	UPROPERTY(EditDefaultsOnly, Category="Item Durability")
	bool bUseDurability;


	UPROPERTY(EditDefaultsOnly, Category="Item Usage")
	EItemUseType UseType;
	UPROPERTY(EditDefaultsOnly, Category="Item Usage")
	EItemRemoveType RemoveType;
	UPROPERTY(EditDefaultsOnly, Category="Item Usage")
	FText TextDocument;
	UPROPERTY(EditDefaultsOnly, Category="Item Usage")
	FString CraftingIDToUnlock;

	UPROPERTY(EditDefaultsOnly, Category="Loot Randomization")
	uint32 DropPercentage;
	UPROPERTY(EditDefaultsOnly, Category="Loot Randomization")
	TArray<UMainItemData*> LootClasses;
	UPROPERTY(EditDefaultsOnly, Category="Loot Randomization")
	uint32 MinRandQuantity;
	UPROPERTY(EditDefaultsOnly, Category="Loot Randomization")
	uint32 MaxRandQuantity;
	UPROPERTY(EditDefaultsOnly, Category="Loot Randomization")
	uint32 MinRandDurability;
	UPROPERTY(EditDefaultsOnly, Category="Loot Randomization")
	uint32 MaxRandDurability;
	
	//UPROPERTY(EditDefaultsOnly, Category="Item Equip")
	//ABaseEquippable* EquippableClass;
	UPROPERTY(EditDefaultsOnly, Category="Item Equip")
	USkeletalMesh* SkeletalMesh;

	//UPROPERTY(EditDefaultsOnly, Category="Item Consumable Buff")
	//ABaseConsumable* ConsumableClass;

	UPROPERTY(EditDefaultsOnly, Category="Item Interactable")
	ABaseItemPickUp* InteractableClass;
	
};
