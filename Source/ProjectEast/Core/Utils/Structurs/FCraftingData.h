#pragma once

#include "FSingleDTItem.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "FCraftingData.generated.h"

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
	ECraftingSubategory Subcategory;
	UPROPERTY(EditDefaultsOnly)
	TArray<ECraftingStation> RequiredStations;
	UPROPERTY(EditDefaultsOnly)
	uint8 CraftingCounter;
	UPROPERTY(EditDefaultsOnly)
	uint8 MaxCount;
};
