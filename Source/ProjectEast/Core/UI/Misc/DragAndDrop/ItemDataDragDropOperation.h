#pragma once
#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDataDragDropOperation.generated.h"


class UItemDataDragAndDropPanel;
class UInventoryCore;
struct FItemData;

UENUM(BlueprintType)
enum class EItemDestination : uint8
{
	InventorySlot UMETA(DisplayName = "InventorySlot"),
	EquipmentSlot UMETA(DisplayName = "EquipmentSlot"),
	VendorSlot UMETA(DisplayName = "VendorSlot"),
	StorageSlot UMETA(DisplayName = "StorageSlot"),
	DropBar UMETA(DisplayName = "DropBar"),
};


UCLASS()
class PROJECTEAST_API UItemDataDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	void ClearDraggableIcon() const;
	void ShowIconWrongSlot() const;
	void ShowIconDrop() const;
	void ShowIconSwap() const;

	UInventoryCore* Inventory;
	UItemDataDragAndDropPanel* ItemDataDragAndDropPanel;

	FItemData* ItemData;
	EItemDestination DraggerFrom;
};
