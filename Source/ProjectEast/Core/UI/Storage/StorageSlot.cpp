#include "StorageSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragAndDropPanel.h"

void UStorageSlot::NativeConstruct()
{
	Super::NativeConstruct();
	SetButtonStyle(CurrentItemData);
	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	
	ButtonItem->OnClicked.AddDynamic(this, &UStorageSlot::OnItemClick);
	ButtonItem->OnHovered.AddDynamic(this, &UStorageSlot::OnItemHovered);
	ButtonItem->OnUnhovered.AddDynamic(this, &UStorageSlot::OnItemUnhovered);
}

void UStorageSlot::NativeDestruct()
{
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
	
	ButtonItem->OnClicked.RemoveDynamic(this, &UStorageSlot::OnItemClick);
	ButtonItem->OnHovered.RemoveDynamic(this, &UStorageSlot::OnItemHovered);
	ButtonItem->OnUnhovered.RemoveDynamic(this, &UStorageSlot::OnItemUnhovered);
	
	Super::NativeDestruct();
}

void UStorageSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (IsUsingGamepad())
		if (IsValid(CachedToolTip))
			SetToolTipPositionAndAlignment();
}

void UStorageSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	if (IsUsingGamepad())
	{
		StopAnimation(AnimationHighlight);
		BorderObject->SetBrushColor(BorderHovered);

		CachedParentWidget->AssignCurrentlyFocusedSlot(this);
		CachedParentWidget->ScrollToSlot(this);

		if (IGamepadControls* GamePadControls = Cast<IGamepadControls>(GetOwningPlayer()))
			GamePadControls->SetCurrentlyFocusedWidget(EWidgetType::Storage);

		//TODO Delay 0.1f

		RefreshGeometryCache();
	}
}

void UStorageSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	OnItemUnhovered();

}

void UStorageSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		Operation->ClearDraggableIcon();
		BorderObject->SetBrushColor(BorderUnHovered);
	}
}

FReply UStorageSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnItemClick();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UStorageSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UStorageSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                        UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (IsValid(PlayerInventory))
			PlayerInventory->SwitchedActivePanel(CurrentItemData->Class.GetDefaultObject()->InventoryPanel);

		UItemDataDragAndDropPanel* DragAndDropPanel = CreateWidget<UItemDataDragAndDropPanel>(
			this, ItemDataDragAndDropPanel);
		DragAndDropPanel->InitializePanel(CurrentItemData->Class.GetDefaultObject()->ImageItem, DraggedImageSize);

		auto Operation = Cast<UItemDataDragDropOperation>(
			UWidgetBlueprintLibrary::CreateDragDropOperation(ItemDataDragDropOperation));
		Operation->DefaultDragVisual = DragAndDropPanel;
		Operation->ItemData = CurrentItemData;
		Operation->Inventory = ActorInventory;
		Operation->ItemDataDragAndDropPanel = DragAndDropPanel;
		Operation->DraggerFrom = EItemDestination::InventorySlot;

		OutOperation = Operation;
	}
}

bool UStorageSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                UDragDropOperation* InOperation)
{
	if (UItemDataDragDropOperation* DragOperation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		switch (DragOperation->DraggerFrom)
		{
		case EItemDestination::InventorySlot:
			DraggedFromInventory(DragOperation, CurrentItemData);
			break;
		case EItemDestination::VendorSlot:
		case EItemDestination::StorageSlot:
			DraggedFromOtherInventory(DragOperation, CurrentItemData);
			break;
		default: ;
		}
	}
	return false;
}

bool UStorageSlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                    UDragDropOperation* InOperation)
{
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		if (InventoryUtility::IsItemClassValid(CurrentItemData))
		{
			switch (Operation->DraggerFrom)
			{
			case EItemDestination::InventorySlot:
				{
					if (InventoryUtility::AreItemsTheSame(CurrentItemData, Operation->ItemData) &&
						InventoryUtility::AreItemsStackable(CurrentItemData, Operation->ItemData))
					{
						Operation->ShowIconWrongSlot();
						BorderObject->SetBrushColor(FLinearColor(0, 737911, 0.0f, 0.028426f));
						return true;
					}
					else
					{
						Operation->ShowIconDrop();
						BorderObject->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
						return true;
					}
				}
			case EItemDestination::VendorSlot:
			case EItemDestination::StorageSlot:
				{
					Operation->ShowIconSwap();
					BorderObject->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
					return true;
				}
			default: ;
			}
		}
		else
		{
			Operation->ShowIconDrop();
			BorderObject->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
			return true;
		}
	}

	return true;
}

FReply UStorageSlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (IsUsingGamepad())
	{
		ButtonItem->SetKeyboardFocus();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UStorageSlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		ShowItemComparison();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UStorageSlot::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		HideItemComparison();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UStorageSlot::ShowItemComparison() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
		if (CurrentItemData->Class.GetDefaultObject()->Rarity != EItemRarity::Consumable)
			if (IsValid(CachedToolTip))
				CachedToolTip->ShowComparisonToolTip();
}

void UStorageSlot::HideItemComparison() const
{
	if (IsValid(CachedToolTip))
		CachedToolTip->HideComparisonToolTip();
}

void UStorageSlot::OnItemClick()
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (InventoryUtility::IsStackableAndHaveStacks(CurrentItemData, 1))
		{
			if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
				WidgetManager->OpenSplitStackPopup(CurrentItemData, nullptr, ActorInventory, PlayerInventory,
												   EInputMethodType::RightClick, EItemDestination::StorageSlot,
												   EItemDestination::InventorySlot, this);
		}
		else
		{
			PlayerInventory->ServerTransferItemFromInventory(PlayerInventory, CurrentItemData,
															 nullptr, EInputMethodType::RightClick, ActorInventory,
															 GetOwningPlayer());
		}
	}
}

void UStorageSlot::OnItemHovered()
{
	if (!IsAnyPopupActive() && !IsUsingGamepad())
	{
		StopAnimation(AnimationHighlight);
		if (IsValid(CachedToolTip))
			CachedToolTip->RemoveFromParent();
		
		CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
		CachedToolTip->InitializeToolTip(CurrentItemData, false);

		ButtonItem->SetToolTip(CachedToolTip);
		BorderObject->SetBrushColor(BorderHovered);
	}
}

void UStorageSlot::OnItemUnhovered()
{
	if (IGamepadControls* GamePadControls = Cast<IGamepadControls>(GetOwningPlayer()))
		GamePadControls->SetCurrentlyFocusedWidget(EWidgetType::None);

	ButtonItem->SetToolTip(nullptr);
	BorderObject->SetBrushColor(BorderUnHovered);
	HideItemComparison();
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
}

FText UStorageSlot::SetQuantity() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))

		if (CurrentItemData->Class.GetDefaultObject()->bIsStackable && CurrentItemData->Quantity > 1)
			return FText::FromString(FString::FromInt(CurrentItemData->Quantity));
	return FText();
}

void UStorageSlot::SetButtonStyle(FItemData* ItemData) const
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		auto Texture = ItemData->Class.GetDefaultObject()->ImageItem;
		if (IsValid(Texture))
		{
			if (ItemData->bIsAlreadyUsed)
				ImageItem->SetBrushTintColor(FSlateColor(FLinearColor(0.223958f, 0.220658f, 0.210761f)));

			ImageItem->SetBrushFromTexture(Texture);
			ImageItem->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
		ImageItem->SetVisibility(ESlateVisibility::Collapsed);
}

void UStorageSlot::EmptySlot()
{
	CurrentItemData = new FItemData();
	CurrentItemData->Index = SlotIndex;

	SetButtonStyle(CurrentItemData);
	ButtonItem->SetToolTip(nullptr);
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
}

void UStorageSlot::OverwriteSlot(FItemData* ItemData)
{
	CurrentItemData = ItemData;
	SetButtonStyle(CurrentItemData);
	RefreshToolTip();
}


void UStorageSlot::InitializeSlot(FItemData* ItemData, UStorageInventory* ParentWidget, UInventoryCore* OwnerInv,
	UPlayerInventory* PlayerInv, int IndexSlot, FVector2D DragImageSize)
{
	CachedParentWidget = ParentWidget;
	ActorInventory = OwnerInv;
	PlayerInventory = PlayerInv;
	SlotIndex = IndexSlot;
	DraggedImageSize = DragImageSize;

	OverwriteSlot(ItemData);
}

void UStorageSlot::HighlightSlot()
{
	if (!HasUserFocusedDescendants(GetOwningPlayer()) && !IsHovered())
		PlayAnimation(AnimationHighlight, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UStorageSlot::RefreshToolTip()
{
	if (IsUsingGamepad())
	{
		if (HasUserFocusedDescendants(GetOwningPlayer()))
		{
			SetToolTip(ButtonItem);
			HideItemComparison();
			if (IsValid(CachedToolTip))
				CachedToolTip->RemoveFromParent();

			CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
			CachedToolTip->InitializeToolTip(CurrentItemData, false);
			CachedToolTip->AddToViewport();
			SetToolTipPositionAndAlignment();
		}
	}
	else
	{
		if (IsHovered())
		{
			SetToolTip(ButtonItem);
			HideItemComparison();
			if (IsValid(CachedToolTip))
				CachedToolTip->RemoveFromParent();

			CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
			CachedToolTip->InitializeToolTip(CurrentItemData, false);
			CachedToolTip->AddToViewport();
			SetToolTip(ButtonItem);
		}
	}
}

void UStorageSlot::RefreshGeometryCache()
{
	if (ButtonItem->HasKeyboardFocus())
	{
		if (IsValid(CachedToolTip))
			CachedToolTip->RemoveFromParent();
		CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
		CachedToolTip->AddToViewport();
		SetToolTipPositionAndAlignment();
	}
}

bool UStorageSlot::IsUsingGamepad() const
{
	if (IsValid(CachedPlayerController))
		return CachedPlayerController->IsUsingGamepad();
	return false;
}

void UStorageSlot::SetToolTipPositionAndAlignment() const
{
	FVector2D HorizontalFirstPixelPosition;
	FVector2D HorizontalFirstViewportPosition;
	FVector2D HorizontalSecondPixelPosition;
	FVector2D HorizontalSecondViewportPosition;

	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(), GetCachedGeometry().GetLocalSize(),
	                                        HorizontalFirstPixelPosition, HorizontalFirstViewportPosition);
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),
	                                        FVector2D(0, GetCachedGeometry().GetLocalSize().Y),
	                                        HorizontalSecondPixelPosition, HorizontalSecondViewportPosition);

	float HorizontalMaxValue = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).X -
		(CachedToolTip->GetDesiredSize().X * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

	bool HorizontalValue = UKismetMathLibrary::InRange_FloatFloat(HorizontalSecondPixelPosition.X, 0.0f,
	                                                              HorizontalMaxValue, true, true);

	float HorizontalPosition = HorizontalValue ? HorizontalSecondViewportPosition.X : HorizontalFirstViewportPosition.X;
	float HorizontalAlignment = HorizontalValue ? 0.0f : 1.0f;


	FVector2D VerticalFirstPixelPosition;
	FVector2D VerticalFirstViewportPosition;
	FVector2D VerticalSecondPixelPosition;
	FVector2D VerticalSecondViewportPosition;

	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),
	                                        FVector2D(GetCachedGeometry().GetLocalSize().X, 0),
	                                        VerticalFirstPixelPosition, VerticalFirstViewportPosition);
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(), GetCachedGeometry().GetLocalSize(),
	                                        VerticalSecondPixelPosition, VerticalSecondViewportPosition);

	float VerticalMaxValue = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).Y -
		(CachedToolTip->GetDesiredSize().Y * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

	bool VerticalValue = UKismetMathLibrary::InRange_FloatFloat(VerticalSecondPixelPosition.Y, 0.0f, VerticalMaxValue,
	                                                            true, true);

	float VerticalPosition = VerticalValue ? VerticalSecondViewportPosition.Y : VerticalFirstViewportPosition.Y;
	float VerticalAlignment = VerticalValue ? 0.0f : 1.0f;

	CachedToolTip->SetPositionInViewport(FVector2D(HorizontalPosition, VerticalPosition), false);
	CachedToolTip->SetAlignmentInViewport(FVector2D(HorizontalAlignment, VerticalAlignment));
	CachedToolTip->SetDesiredSizeInViewport(CachedToolTip->GetDesiredSize());
}

bool UStorageSlot::IsAnyPopupActive() const
{
	if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
		return WidgetManager->GetCurrentPopupType() != EWidgetType::Vendor;

	return false;
}

void UStorageSlot::DraggedFromInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const
{
	if (!InventoryUtility::IsItemClassValid(ItemData) || (InventoryUtility::AreItemsTheSame(
			Operation->ItemData, ItemData) &&
		InventoryUtility::AreItemsStackable(Operation->ItemData, ItemData)))
	{
		switch (PlayerInventory->GetItemRemoveType(Operation->ItemData))
		{
		case EItemRemoveType::Default:
			{
				if (InventoryUtility::IsStackableAndHaveStacks(Operation->ItemData, 1))
				{
					if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
						WidgetManager->OpenSplitStackPopup(Operation->ItemData, ItemData, PlayerInventory,
						                                   ActorInventory,
						                                   EInputMethodType::DragAndDrop, Operation->DraggerFrom,
						                                   EItemDestination::StorageSlot, nullptr);
				}
				else
				{
					PlayerInventory->ServerTransferItemFromInventory(ActorInventory, Operation->ItemData,
					                                                 ItemData, EInputMethodType::DragAndDrop,
					                                                 PlayerInventory, GetOwningPlayer());
				}
			}
			break;
		case EItemRemoveType::OnConfirmation:
			{
				if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
					WidgetManager->OpenConfirmationPopup("Are you sure you want to remove?", Operation->ItemData,
					                                     ItemData,
					                                     PlayerInventory, ActorInventory, EInputMethodType::DragAndDrop,
					                                     Operation->DraggerFrom, EItemDestination::StorageSlot,
					                                     nullptr);
				BorderObject->SetBrushColor(BorderUnHovered);
			}
			break;
		case EItemRemoveType::CannotBeRemoved:
			{
				if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
					WidgetManager->DisplayMessageNotify("Item cannot be Removed.");
			}
			break;
		}
	}
}

void UStorageSlot::DraggedFromOtherInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const
{
	if (Operation->ItemData->Index != ItemData->Index)
		PlayerInventory->ServerMoveItemToSlot(ActorInventory, EInventoryPanels::P1, Operation->ItemData->Index,
		                                      ItemData->Index);
}
