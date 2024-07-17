#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "MainItemData.generated.h"

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
	EItemSlot EquipmentSlot;
	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	EItemUseType UseType;

	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	uint32 RequiredLevel;

	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	TArray<FItemsStatRow> Stats;
	
	UPROPERTY(EditDefaultsOnly, Category="Item Stats")
	float Weight;
	UPROPERTY(EditDefaultsOnly, Category="Item Stats")
	float Value;

	UPROPERTY(EditDefaultsOnly, Category="Item Stacking")
	bool bUseDurability;
	UPROPERTY(EditDefaultsOnly, Category="Item Stacking")
	bool bAlredyUsed;

	UPROPERTY(EditDefaultsOnly, Category="Item Details")
	uint32 Durability;

	UPROPERTY(EditDefaultsOnly, Category="Item Stacking")
	bool bIsStackable;
	UPROPERTY(EditDefaultsOnly, Category="Item Stacking")
	EItemRemoveType RemoveType;
};
