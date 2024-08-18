#include "EquipmentSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragAndDropPanel.h"

void UEquipmentSlot::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerInventory = Cast<AMainPlayerController>(GetOwningPlayer())->GetPlayerInventory();
	CachedPlayerEquipment = Cast<AMainPlayerController>(GetOwningPlayer())->GetPlayerEquipment();

	ButtonItem->OnClicked.AddDynamic(this, &UEquipmentSlot::OnRightClick);
	ButtonItem->OnHovered.AddDynamic(this, &UEquipmentSlot::OnHovered);
	ButtonItem->OnUnhovered.AddDynamic(this, &UEquipmentSlot::OnUnhovered);
}

void UEquipmentSlot::NativeDestruct()
{
	Super::NativeDestruct();
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();

	ButtonItem->OnClicked.RemoveDynamic(this, &UEquipmentSlot::OnRightClick);
	ButtonItem->OnHovered.RemoveDynamic(this, &UEquipmentSlot::OnHovered);
	ButtonItem->OnUnhovered.RemoveDynamic(this, &UEquipmentSlot::OnUnhovered);
}

void UEquipmentSlot::NativePreConstruct()
{
	Super::NativePreConstruct();
	ImageItem->SetBrush(EmptySlotStyle);
	OverlayLabel->SetVisibility(GetLabelVisibility());
	TextLabel->SetText(GetLabelCharacter());
}

void UEquipmentSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	if (IsUsingGamepad())
	{
		BorderImage->SetBrushTintColor(BorderHovered);

		CachedEquipmentPanel->AssignCurrentlyFocusedSlot(CurrentItemData->EquipmentSlot);

		if (IGamepadControls* GamePadControls = Cast<IGamepadControls>(GetOwningPlayer()))
			GamePadControls->SetCurrentlyFocusedWidget(EWidgetType::Equipment);
		if (IsValid(CachedToolTip))
			CachedToolTip->RemoveFromParent();

		CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
		CachedToolTip->InitializeToolTip(CurrentItemData, false);
		CachedToolTip->AddToViewport();
		SetToolTipPositionAndAlignment();
	}
}

void UEquipmentSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	OnUnhovered();
}

void UEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		Operation->ClearDraggableIcon();
		BorderImage->SetColorAndOpacity(BorderUnHovered);
	}
}

FReply UEquipmentSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UEquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClick();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void UEquipmentSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                          UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (IsValid(CachedPlayerInventory))
			CachedPlayerInventory->SwitchedActivePanel(CurrentItemData->Class.GetDefaultObject()->InventoryPanel);

		UItemDataDragAndDropPanel* DragAndDropPanel = CreateWidget<UItemDataDragAndDropPanel>(
			this, ItemDataDragAndDropPanel);
		DragAndDropPanel->InitializePanel(CurrentItemData->Class.GetDefaultObject()->ImageItem, FVector2D(100, 100));

		auto Operation = Cast<UItemDataDragDropOperation>(
			UWidgetBlueprintLibrary::CreateDragDropOperation(ItemDataDragDropOperation));
		Operation->DefaultDragVisual = DragAndDropPanel;
		Operation->ItemDataDragAndDropPanel = DragAndDropPanel;
		Operation->ItemData = CurrentItemData;
		Operation->DraggerFrom = EItemDestination::EquipmentSlot;

		OutOperation = Operation;
	}
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                  UDragDropOperation* InOperation)
{
	if (UItemDataDragDropOperation* DragOperation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		if (!InventoryUtility::AreItemSlotsEqual(DragOperation->ItemData, CurrentItemData))
			if (CachedPlayerEquipment->bIsEnableOffHand)
				if(!InventoryUtility::CanWeaponsBeSwapped(DragOperation->ItemData, CurrentItemData))
					return true;
 
		switch (DragOperation->DraggerFrom)
		{
		case EItemDestination::InventorySlot:
			{
				CachedPlayerEquipment->ServerTransferItemFromInventory(DragOperation->ItemData, CurrentItemData,DragOperation->Inventory,EInputMethodType::DragAndDrop);
				BorderImage->SetColorAndOpacity(BorderUnHovered);
				return true;
			}
		case EItemDestination::EquipmentSlot:
			{
				if (InventoryUtility::IsItemClassValid(CurrentItemData))
				{
					if (InventoryUtility::AreWeaponTypesEqual(DragOperation->ItemData, CurrentItemData))
						CachedPlayerEquipment->ServerTransferItemFromEquipment(DragOperation->ItemData, CurrentItemData);
				}
				else
					CachedPlayerEquipment->ServerTransferItemFromEquipment(DragOperation->ItemData, CurrentItemData);
			}
		default: ;
		}
	}
	return true;
}

bool UEquipmentSlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                      UDragDropOperation* InOperation)
{
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		if (InventoryUtility::AreItemSlotsEqual(CurrentItemData, Operation->ItemData))
			return TryWeaponsSwapped(Operation);

		if (CachedPlayerEquipment->bIsEnableOffHand)
		{
			if (InventoryUtility::CanWeaponsBeSwapped(Operation->ItemData, CurrentItemData))
				return TryWeaponsSwapped(Operation);
			return ReturnWrongSlot(Operation);
		}
		return ReturnWrongSlot(Operation);
	}
	return true;
}

void UEquipmentSlot::EmptySlot() 
{
	CurrentItemData = InventoryUtility::CopyItemData(EmptyItemData);
	CurrentItemData->EquipmentSlot = EquipmentSLot;
	SetButtonStyle(CurrentItemData);
	ButtonItem->SetToolTip(nullptr);
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
}

void UEquipmentSlot::OnRightClick()
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		CachedPlayerInventory->ServerTransferItemFromEquipment(CurrentItemData, new FItemData(), EInputMethodType::RightClick, CachedPlayerEquipment);
	}
}

void UEquipmentSlot::RefreshToolTip()
{
	if (IsUsingGamepad())
	{
		if (HasUserFocusedDescendants(GetOwningPlayer()))
		{
			ButtonItem->SetToolTip(nullptr);
			if (IsValid(CachedToolTip))
				CachedToolTip->RemoveFromParent();

			CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
			CachedToolTip->InitializeToolTip(CurrentItemData, false);
			CachedToolTip->AddToViewport(1);
			SetToolTipPositionAndAlignment();
		}
	}
	else
	{
		if (IsHovered())
		{
			SetToolTip(ButtonItem);
			if (IsValid(CachedToolTip))
				CachedToolTip->RemoveFromParent();

			CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
			CachedToolTip->InitializeToolTip(CurrentItemData, false);
			ButtonItem->SetToolTip(CachedToolTip);
		}
	}
}

void UEquipmentSlot::OnDropPressed()
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
		if (GetActiveWidgetType() == EWidgetType::Inventory)
			DropOnTheGround();
}

void UEquipmentSlot::SetItemQuantity() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (CurrentItemData->Class.GetDefaultObject()->bIsStackable && CurrentItemData->Quantity > 1)
		{
			TextQuantity->SetText(FText::FromString(FString::FromInt(CurrentItemData->Quantity)));
			TextQuantity->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
			TextQuantity->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
		TextQuantity->SetVisibility(ESlateVisibility::Collapsed);
}

void UEquipmentSlot::DropOnTheGround()
{
	switch (CachedPlayerInventory->GetItemRemoveType(CurrentItemData))
	{
	case EItemRemoveType::Default:
		{
			if (InventoryUtility::IsStackableAndHaveStacks(CurrentItemData, 1))
			{
				if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
					WidgetManager->OpenSplitStackPopup(CurrentItemData, new FItemData(), nullptr, CachedPlayerInventory,
					                                   EInputMethodType::RightClick,
					                                   EItemDestination::EquipmentSlot, EItemDestination::DropBar,
					                                   this);
			}
			else
				CachedPlayerInventory->ServerDropItemOnTheGround(CurrentItemData, EItemDestination::EquipmentSlot,
				                                                 GetOwningPlayer());
		}
	case EItemRemoveType::OnConfirmation:
		if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
			WidgetManager->OpenConfirmationPopup("Are you sure you want to remove?", CurrentItemData, new FItemData(),
			                                     nullptr, CachedPlayerInventory, EInputMethodType::RightClick,
			                                     EItemDestination::EquipmentSlot,
			                                     EItemDestination::DropBar, this);
		break;
	case EItemRemoveType::CannotBeRemoved:
		if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
			WidgetManager->DisplayMessageNotify("Item cannot be Removed.");
		break;
	}
}

void UEquipmentSlot::SetToolTipPositionAndAlignment() const
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

void UEquipmentSlot::SetButtonStyle(FItemData* ItemData) const
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		auto Texture = ItemData->Class.GetDefaultObject()->ImageItem;
		if (IsValid(Texture))
		{
			ImageItem->SetBrushFromTexture(Texture);
			ImageItem->SetBrushTintColor(FLinearColor(1,1,1));
		}
	}
	else
		ImageItem->SetBrush(EmptySlotStyle);
	
	SetItemQuantity();
}

void UEquipmentSlot::OverwriteSlot(UEquipmentPanel* EquipmentPanel, FItemData* ItemData)
{
	CachedEquipmentPanel = EquipmentPanel;

	if (InventoryUtility::IsItemClassValid(ItemData))
		CurrentItemData = InventoryUtility::CopyItemData(ItemData);
	else
	{
		CurrentItemData = InventoryUtility::CopyItemData(EmptyItemData);
		CurrentItemData->EquipmentSlot = EquipmentSLot;
	}

	SetButtonStyle(CurrentItemData);
	RefreshToolTip();
}

ESlateVisibility UEquipmentSlot::GetLabelVisibility() const
{
	return LabelCharacterText.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible;
}

FText UEquipmentSlot::GetLabelCharacter()
{
	return LabelCharacterText;
}

EWidgetType UEquipmentSlot::GetActiveWidgetType() const
{
	if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
		return WidgetManager->GetActiveWidget();
	return EWidgetType::None;
}

bool UEquipmentSlot::IsUsingGamepad() const
{
	if (IGamepadControls* GamepadControls = Cast<IGamepadControls>(GetOwningPlayer()))
		return GamepadControls->IsUsingGamepad();
	return false;
}

void UEquipmentSlot::OnHovered()
{
	if (!IsUsingGamepad())
	{
		if (IsValid(CachedToolTip))
			CachedToolTip->RemoveFromParent();

		CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
		CachedToolTip->InitializeToolTip(CurrentItemData, false);

		ButtonItem->SetToolTip(CachedToolTip);
		BorderImage->SetColorAndOpacity(BorderHovered);
	}
}

void UEquipmentSlot::OnUnhovered()
{
	if (IGamepadControls* GamePadControls = Cast<IGamepadControls>(GetOwningPlayer()))
		GamePadControls->SetCurrentlyFocusedWidget(EWidgetType::None);

	ButtonItem->SetToolTip(nullptr);
	BorderImage->SetColorAndOpacity(BorderUnHovered);
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
}

bool UEquipmentSlot::TryWeaponsSwapped(UItemDataDragDropOperation* Operation)
{
	if (CachedPlayerEquipment->CanItemBeEquipped(Operation->ItemData).Get<0>())
	{
		Operation->ShowIconSwap();
		BorderImage->SetColorAndOpacity(GoodColor);
		return true;
	}

	Operation->ShowIconWrongSlot();
	BorderImage->SetColorAndOpacity(BadColor);
	return true;
}

bool UEquipmentSlot::ReturnWrongSlot(UItemDataDragDropOperation* Operation)
{
	Operation->ShowIconWrongSlot();
	BorderImage->SetColorAndOpacity(BadColor);
	return true;
}
