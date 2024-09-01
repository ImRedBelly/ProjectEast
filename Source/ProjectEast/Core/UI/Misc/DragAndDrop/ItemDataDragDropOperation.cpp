#include "ItemDataDragDropOperation.h"
#include "ItemDataDragAndDropPanel.h"
#include "Components/Image.h"

void UItemDataDragDropOperation::ClearDraggableIcon() const
{
	if (IsValid(ItemDataDragAndDropPanel))
	{
		ItemDataDragAndDropPanel->WrongSlotImage->SetVisibility(ESlateVisibility::Hidden);
		ItemDataDragAndDropPanel->DropSlotImage->SetVisibility(ESlateVisibility::Hidden);
		ItemDataDragAndDropPanel->SwapSlotImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemDataDragDropOperation::ShowIconWrongSlot() const
{
	ClearDraggableIcon();
	ItemDataDragAndDropPanel->WrongSlotImage->SetVisibility(ESlateVisibility::Visible);
}

void UItemDataDragDropOperation::ShowIconDrop() const
{
	ClearDraggableIcon();
	ItemDataDragAndDropPanel->DropSlotImage->SetVisibility(ESlateVisibility::Visible);
}

void UItemDataDragDropOperation::ShowIconSwap() const
{
	ClearDraggableIcon();
	ItemDataDragAndDropPanel->SwapSlotImage->SetVisibility(ESlateVisibility::Visible);
}
