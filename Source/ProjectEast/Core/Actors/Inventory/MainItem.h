#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "MainItem.generated.h"

UCLASS()
class PROJECTEAST_API AMainItem : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* ImageItem;

	UPROPERTY(EditDefaultsOnly)
	FText NameItem;
	
	EInventoryPanels InventoryPanel;
	EItemsType Type;
	EItemRarity Rarity;
	float Weight;
	float Value;
	bool bIsStackable;
	int32 Quantity;
};
