#include "CraftingQueue.h"

#include "CraftingQueueSlot.h"

bool UCraftingQueue::CanBeFocused() const
{
	return IsValid(HorizontalBoxSlots->GetChildAt(0));
}

void UCraftingQueue::InitializeQueue(UCraftingCore* InCraftingCore)
{
	CraftingStation = InCraftingCore;
	BindEventDispatchers();
	CreateSlot();
}

void UCraftingQueue::SetWidgetToTransferFocus(UUserWidget* UserWidget)
{
	WidgetToTransferFocus = UserWidget;
}

void UCraftingQueue::SetFocusedSlotIndex(int32 Index)
{
	CurrentlyFocusedSlot = Index;
}

void UCraftingQueue::NativeDestruct()
{
	UnbindEventDispatchers();
	Super::NativeDestruct();
}

FReply UCraftingQueue::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	auto FirstChild = HorizontalBoxSlots->GetChildAt(0);
	if (IsValid(FirstChild))
	{
		FirstChild->SetKeyboardFocus();
	}
	return FReply::Handled();
}

void UCraftingQueue::BindEventDispatchers()
{
	if (IsValid(CraftingStation))
	{
		CraftingStation->OnRefreshed.AddDynamic(this, &UCraftingQueue::CreateSlot);
		CraftingStation->OnAddedToQueue.AddDynamic(this, &UCraftingQueue::ItemAddedToQueue);
		CraftingStation->OnRemoveFromQueue.AddDynamic(this, &UCraftingQueue::ItemRemovedFromQueue);
		CraftingStation->OnClearedQueue.AddDynamic(this, &UCraftingQueue::CreateSlot);
	}
}

void UCraftingQueue::UnbindEventDispatchers()
{
	if (IsValid(CraftingStation))
	{
		CraftingStation->OnRefreshed.RemoveDynamic(this, &UCraftingQueue::CreateSlot);
		CraftingStation->OnAddedToQueue.RemoveDynamic(this, &UCraftingQueue::ItemAddedToQueue);
		CraftingStation->OnRemoveFromQueue.RemoveDynamic(this, &UCraftingQueue::ItemRemovedFromQueue);
		CraftingStation->OnClearedQueue.RemoveDynamic(this, &UCraftingQueue::CreateSlot);
	}
}

void UCraftingQueue::CreateSlot()
{
	HorizontalBoxSlots->ClearChildren();
	TArray<FCraftingData*> CraftingQueue = CraftingStation->GetCraftingQueue();
	for (int i = 0; i < CraftingQueue.Num(); ++i)
	{
		UCraftingQueueSlot* NewSlot = CreateWidget<UCraftingQueueSlot>(this, CraftingQueueSlotClass);
		NewSlot->InitializeSlot(CraftingStation, CraftingQueue[i], this, i);
		HorizontalBoxSlots->AddChildToHorizontalBox(NewSlot);
	}

	if (HasKeyboardFocus() || HasUserFocusedDescendants(GetOwningPlayer()))
		SetFocusToSlot(CurrentlyFocusedSlot);
}

void UCraftingQueue::ItemAddedToQueue(FCraftingData& CraftingData, int32 QueueIndex)
{
	CreateSlot();
}

void UCraftingQueue::ItemRemovedFromQueue(FCraftingData& CraftingData, int32 QueueIndex)
{
	if (QueueIndex <= CurrentlyFocusedSlot)
		CurrentlyFocusedSlot--;
	CreateSlot();
}

void UCraftingQueue::SetFocusToSlot(int32 Index) const
{
	auto FindChild = HorizontalBoxSlots->GetChildAt(Index);
	if (IsValid(FindChild))
	{
		FindChild->SetKeyboardFocus();
	}
	else
	{
		auto FirstChild = HorizontalBoxSlots->GetChildAt(0);
		if (IsValid(FirstChild))
		{
			FirstChild->SetKeyboardFocus();
		}
		else
		{
			WidgetToTransferFocus->SetKeyboardFocus();
		}
	}
}
