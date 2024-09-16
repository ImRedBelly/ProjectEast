#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ItemDataDragDropOperation.generated.h"


class UItemDataDragAndDropPanel;
class UInventoryCore;
struct FItemData;


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
