#include "VendorSlot.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragAndDropPanel.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragDropOperation.h"
#include "ProjectEast/Core/UI/ToolTip/ToolTip.h"
#include "ProjectEast/Core/UI/Vendor/VendorInventory.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"


void UVendorSlot::InitializeSlot(UVendorInventory* InParentWidget, UInventoryCore* InActorInventory,
                                 UInventoryCore* InPlayerInventory, UWidgetManager* InWidgetManager, int32 InSlotIndex)
{
	ParentWidgetRef = InParentWidget;
	ActorInventory = InActorInventory;
	PlayerInventory = InPlayerInventory;
	WidgetManager = InWidgetManager;
	SlotIndex = InSlotIndex;
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);
	SetButtonStyle(CurrentItemData);

	ButtonItem->OnClicked.AddDynamic(this, &UVendorSlot::OnClickedButton);
	ButtonItem->OnHovered.AddDynamic(this, &UVendorSlot::OnHoveredButton);
	ButtonItem->OnUnhovered.AddDynamic(this, &UVendorSlot::OnUnhoveredButton);
}

void UVendorSlot::NativeDestruct()
{
	Super::NativeDestruct();
	if (IsValid(CachedToolTip))
	{
		CachedToolTip->RemoveFromParent();
		CachedToolTip = nullptr;
	}

	if (IsValid(ButtonItem))
	{
		ButtonItem->OnClicked.RemoveDynamic(this, &UVendorSlot::OnClickedButton);
		ButtonItem->OnHovered.RemoveDynamic(this, &UVendorSlot::OnHoveredButton);
		ButtonItem->OnUnhovered.RemoveDynamic(this, &UVendorSlot::OnUnhoveredButton);
	}
}

void UVendorSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (IconButtonGameModule->IsUsingGamepad() && IsValid(CachedToolTip))
	{
		SetToolTipPositionAndAlignment();
	}
}

void UVendorSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	if (IsUsingGamepad())
	{
		StopAnimation(AnimationHighlight);
		BorderSlot->SetBrushColor(BorderHovered);
		ParentWidgetRef->AssignCurrentlyFocusedSlot(this);
		ParentWidgetRef->ScrollToSlot(this);
		WidgetManager->SetCurrentlyFocusedWidget(EWidgetType::Vendor);
		RefreshGeometryCache();
	}
}

void UVendorSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	OnUnhoveredButton();
}

void UVendorSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (UItemDataDragDropOperation* Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		Operation->ClearDraggableIcon();
		BorderSlot->SetBrushColor(BorderUnHovered);
	}
}

FReply UVendorSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UVendorSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClick();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UVendorSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
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
		Operation->DraggerFrom = EItemDestination::VendorSlot;
		OutOperation = Operation;
	}
}

bool UVendorSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                               UDragDropOperation* InOperation)
{
	if (UItemDataDragDropOperation* DragOperation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		switch (DragOperation->DraggerFrom)
		{
		case EItemDestination::InventorySlot:
			DraggedFromInventory(DragOperation, CurrentItemData);
			return true;
		case EItemDestination::VendorSlot:
		case EItemDestination::StorageSlot:
			DraggedFromOtherInventory(DragOperation, CurrentItemData);
			return true;
		default: ;
		}
	}
	return false;
}

bool UVendorSlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
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
						BorderSlot->SetBrushColor(FLinearColor(0.737911, 0.0f, 0.028426f));
						return true;
					}
					else
					{
						Operation->ShowIconDrop();
						BorderSlot->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
						return true;
					}
				}
			case EItemDestination::VendorSlot:
			case EItemDestination::StorageSlot:
				{
					Operation->ShowIconSwap();
					BorderSlot->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
					return true;
				}
			default: ;
			}
		}
		else
		{
			Operation->ShowIconDrop();
			BorderSlot->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
			return true;
		}
	}

	return true;
}

FReply UVendorSlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (IsUsingGamepad())
	{
		ButtonItem->SetKeyboardFocus();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UVendorSlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		ShowItemComparison();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UVendorSlot::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		HideItemComparison();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UVendorSlot::OnRightClick()
{
	OnClickedButton();
}

void UVendorSlot::OnClickedButton()
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (InventoryUtility::IsStackableAndHaveStacks(CurrentItemData, 1))
		{
			if (WidgetManager)
				WidgetManager->OpenSplitStackPopup(CurrentItemData, new FItemData(), ActorInventory, PlayerInventory,
				                                   EInputMethodType::RightClick, EItemDestination::VendorSlot,
				                                   EItemDestination::InventorySlot, this);
		}
		else
		{
			PlayerInventory->ServerTransferItemFromInventory(PlayerInventory, CurrentItemData,
			                                                 new FItemData(), EInputMethodType::RightClick,
			                                                 ActorInventory,
			                                                 GetOwningPlayer());
		}
	}
}

void UVendorSlot::OnHoveredButton()
{
	if (!IsAnyPopupActive() && !IsUsingGamepad())
	{
		StopAnimation(AnimationHighlight);
		if (IsValid(CachedToolTip))
		{
			CachedToolTip->RemoveFromParent();
			CachedToolTip = nullptr;
		}

		CachedToolTip = CreateWidget<UToolTip>(this, ToolTipClass);
		CachedToolTip->InitializeToolTip(CurrentItemData, false);

		ButtonItem->SetToolTip(CachedToolTip);
		BorderSlot->SetBrushColor(BorderHovered);
	}
}

void UVendorSlot::OnUnhoveredButton()
{
	WidgetManager->SetCurrentlyFocusedWidget(EWidgetType::None);

	ButtonItem->SetToolTip(nullptr);
	BorderSlot->SetBrushColor(BorderUnHovered);
	HideItemComparison();
	if (IsValid(CachedToolTip))
	{
		CachedToolTip->RemoveFromParent();
		CachedToolTip = nullptr;
	}
}

void UVendorSlot::ShowItemComparison() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (CurrentItemData->Class.GetDefaultObject()->Rarity == EItemRarity::Consumable)
		{
			if (IsValid(CachedToolTip))
			{
				CachedToolTip->ShowComparisonToolTip();
			}
		}
	}
}

void UVendorSlot::HideItemComparison() const
{
	if (IsValid(CachedToolTip))
	{
		CachedToolTip->HideComparisonToolTip();
	}
}

FText UVendorSlot::GetQuantity() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))

		if (CurrentItemData->Class.GetDefaultObject()->bIsStackable && CurrentItemData->Quantity > 1)
			return FText::FromString(FString::FromInt(CurrentItemData->Quantity));
	return FText();
}

void UVendorSlot::SetButtonStyle(FItemData* ItemData) const
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

void UVendorSlot::OverwriteSlot(FItemData* ItemData)
{
	CurrentItemData = ItemData;
	TextQuantity->SetText(GetQuantity());
	SetButtonStyle(CurrentItemData);
	RefreshToolTip();
}

void UVendorSlot::EmptySlot()
{
	CurrentItemData = new FItemData();
	CurrentItemData->Index = SlotIndex;

	SetButtonStyle(CurrentItemData);
	ButtonItem->SetToolTip(nullptr);
	if (IsValid(CachedToolTip))
	{
		CachedToolTip->RemoveFromParent();
		CachedToolTip = nullptr;
	}
}

void UVendorSlot::HighlightSlot()
{
	if (!HasUserFocusedDescendants(GetOwningPlayer()) && IsHovered())
	{
		PlayAnimation(AnimationHighlight);
	}
}

void UVendorSlot::RefreshToolTip()
{
	if (IsUsingGamepad())
	{
		if (HasUserFocusedDescendants(GetOwningPlayer()))
		{
			ButtonItem->SetToolTip(nullptr);
			HideItemComparison();
			if (IsValid(CachedToolTip))
			{
				CachedToolTip->RemoveFromParent();
				CachedToolTip = nullptr;
			}

			CachedToolTip = CreateWidget<UToolTip>(this, ToolTipClass);
			CachedToolTip->InitializeToolTip(CurrentItemData, false);
			CachedToolTip->AddToViewport(1);
			SetToolTipPositionAndAlignment();
		}
	}
	else
	{
		if (IsHovered())
		{
			ButtonItem->SetToolTip(nullptr);
			HideItemComparison();
			if (IsValid(CachedToolTip))
			{
				CachedToolTip->RemoveFromParent();
				CachedToolTip = nullptr;
			}

			CachedToolTip = CreateWidget<UToolTip>(this, ToolTipClass);
			CachedToolTip->InitializeToolTip(CurrentItemData, false);
			ButtonItem->SetToolTip(CachedToolTip);
		}
	}
}

void UVendorSlot::RefreshGeometryCache()
{
	if (ButtonItem->HasKeyboardFocus())
	{
		if (IsValid(CachedToolTip))
		{
			CachedToolTip->RemoveFromParent();
			CachedToolTip = nullptr;
		}
		CachedToolTip = CreateWidget<UToolTip>(this, ToolTipClass);
		CachedToolTip->AddToViewport();
		SetToolTipPositionAndAlignment();
	}
}

void UVendorSlot::SetToolTipPositionAndAlignment() const
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

bool UVendorSlot::IsUsingGamepad() const
{
	return IconButtonGameModule->IsUsingGamepad();
}

bool UVendorSlot::IsAnyPopupActive() const
{
	if (IsValid(WidgetManager))
		return WidgetManager->GetCurrentPopupType() != EWidgetType::None;

	return false;
}

void UVendorSlot::DraggedFromInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const
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
					if (WidgetManager)
						WidgetManager->OpenSplitStackPopup(Operation->ItemData, ItemData, PlayerInventory,
						                                   ActorInventory,
						                                   EInputMethodType::DragAndDrop, Operation->DraggerFrom,
						                                   EItemDestination::VendorSlot, nullptr);
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
				BorderSlot->SetBrushColor(BorderUnHovered);
				if (WidgetManager)
					WidgetManager->OpenConfirmationPopup("Are you sure you want to remove?", Operation->ItemData,
					                                     ItemData,
					                                     PlayerInventory, ActorInventory, EInputMethodType::DragAndDrop,
					                                     Operation->DraggerFrom, EItemDestination::VendorSlot,
					                                     nullptr);
			}
			break;
		case EItemRemoveType::CannotBeRemoved:
			{
				if (WidgetManager)
					WidgetManager->DisplayMessage("Item cannot be Removed.");
			}
			break;
		}
	}
}

void UVendorSlot::DraggedFromOtherInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const
{
	if (Operation->ItemData->Index != ItemData->Index)
		PlayerInventory->ServerMoveItemToSlot(ActorInventory, EInventoryPanels::P1, Operation->ItemData->Index,
		                                      ItemData->Index);
}
