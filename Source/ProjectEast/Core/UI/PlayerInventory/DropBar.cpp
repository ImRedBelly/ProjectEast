#include "DropBar.h"
#include "Components/Image.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragDropOperation.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UDropBar::NativeConstruct()
{
	Super::NativeConstruct();
	auto PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	CachedWidgetManager = PlayerController->GetWidgetManager();
	CachedPlayerInventory = PlayerController->GetPlayerInventory();
}

void UDropBar::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		Operation->ClearDraggableIcon();
		DropImage->SetColorAndOpacity(FLinearColor(0.072272f, 0.072272f, 0.072272f));
		BorderObject->SetBrushColor(FLinearColor(0.346704f, 0.346704f, 0.346704f));
	}
}

bool UDropBar::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                UDragDropOperation* InOperation)
{
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		if (InventoryUtility::CanItemBeRemoved(Operation->ItemData))
		{
			Operation->ShowIconDrop();
			DropImage->SetColorAndOpacity(FLinearColor(0.318547f, 0.318547f, 0.318547f));
			BorderObject->SetBrushColor(FLinearColor(0.012667f, 0.239583f, 0.0f));
			return true;
		}
		else
		{
			Operation->ShowIconWrongSlot();
			DropImage->SetColorAndOpacity(FLinearColor(0.318547f, 0.318547f, 0.318547f));
			BorderObject->SetBrushColor(FLinearColor(0.239583f, 0.014031f, 0.0f));
			return true;
		}
	}
	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UDropBar::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                            UDragDropOperation* InOperation)
{
	DropImage->SetColorAndOpacity(FLinearColor(0.072272f, 0.072272f, 0.072272f));
	BorderObject->SetBrushColor(FLinearColor(0.346704f, 0.346704f, 0.346704f));
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		switch (CachedPlayerInventory->GetItemRemoveType(Operation->ItemData))
		{
		case EItemRemoveType::Default:
			if (InventoryUtility::IsStackableAndHaveStacks(Operation->ItemData, 1))
			{
				CachedWidgetManager->OpenSplitStackPopup(Operation->ItemData, new FItemData(), nullptr,
				CachedPlayerInventory,EInputMethodType::DragAndDrop, Operation->DraggerFrom,
				EItemDestination::DropBar, nullptr);
			}
			else
			{
				CachedPlayerInventory->ServerDropItemOnTheGround(Operation->ItemData,Operation->DraggerFrom, GetOwningPlayer());
			}
			break;
		case EItemRemoveType::OnConfirmation:
			{
				CachedWidgetManager->OpenConfirmationPopup("Are you sure you want to remove?", Operation->ItemData,
				nullptr, nullptr, CachedPlayerInventory,EInputMethodType::DragAndDrop,
				Operation->DraggerFrom, EItemDestination::DropBar, nullptr);
			}
			break;
		case EItemRemoveType::CannotBeRemoved:
			CachedWidgetManager->DisplayMessage("Item cannot be Removed.");
			break;
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
