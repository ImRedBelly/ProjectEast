#include "CraftingQueue.h"

bool UCraftingQueue::CanBeFocused() const
{
	return IsValid(HorizontalBoxSlots->GetChildAt(0));
}

void UCraftingQueue::SetWidgetToTransferFocus(UUserWidget* UserWidget)
{
	WidgetToTransferFocus = UserWidget;
}
