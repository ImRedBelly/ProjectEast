#include "Components/Button.h"
#include "Components/Image.h"
#include "PlayerInventorySlot.h"
#include "Components/TextBlock.h"
#include "PlayerInventoryWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Actors/Interfaces/GamepadControls.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragAndDropPanel.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragDropOperation.h"

void UPlayerInventorySlot::InitializeSlot(FItemData* ItemData, UInventoryCore* ReceiverInventory, UPlayerInventoryWidget* ParentWidget,
	UPlayerEquipment* PlayerEquipment, UPlayerInventory* PlayerInventory, FVector2D DragImageSize, uint32 IndexSlot)
{
	CachedReceiverInventory = ReceiverInventory;
	CachedPlayerInventoryWidget = ParentWidget;
	CachedPlayerEquipment = PlayerEquipment;
	CachedPlayerInventory = PlayerInventory;
	ImageSize = DragImageSize;
	SlotIndex = IndexSlot;
	
	OverwriteSlot(ItemData);
}

void UPlayerInventorySlot::HighlightSlot()
{
	if(!HasUserFocusedDescendants(GetOwningPlayer()) && !IsHovered())
		PlayAnimation(AnimationHighlight, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UPlayerInventorySlot::OnInitialInput()
{
	bIsInitialInputDelay = true;
	GetWorld()->GetTimerManager().ClearTimer(DelayTimer);
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &UPlayerInventorySlot::OffInitialInput, 0.5f, false);
}

void UPlayerInventorySlot::OffInitialInput()
{
	bIsInitialInputDelay = false;
}

void UPlayerInventorySlot::InitializeInventorySlot(UPlayerEquipment* PlayerEquipment, UPlayerInventory* PlayerInventory,
                                                   UInventoryCore* ReceiverInventory)
{
	CachedPlayerEquipment = PlayerEquipment;
	CachedPlayerInventory = PlayerInventory;
	CachedReceiverInventory = ReceiverInventory;
}

void UPlayerInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	
	ButtonItem->OnClicked.AddDynamic(this, &UPlayerInventorySlot::OnRightClick);
	ButtonItem->OnHovered.AddDynamic(this, &UPlayerInventorySlot::OnHovered);
	ButtonItem->OnUnhovered.AddDynamic(this, &UPlayerInventorySlot::OnUnhovered);
}

void UPlayerInventorySlot::NativeDestruct()
{
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();

	ButtonItem->OnClicked.RemoveDynamic(this, &UPlayerInventorySlot::OnRightClick);
	ButtonItem->OnHovered.RemoveDynamic(this, &UPlayerInventorySlot::OnHovered);
	ButtonItem->OnUnhovered.RemoveDynamic(this, &UPlayerInventorySlot::OnUnhovered);
	Super::NativeDestruct();
}

void UPlayerInventorySlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsUsingGamepad() && IsValid(CachedToolTip))
		SetToolTipPositionAndAlignment();
}

void UPlayerInventorySlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	if (IsUsingGamepad())
	{
		StopAnimation(AnimationHighlight);
		BorderObject->SetBrushColor(BorderHovered);
		
		CachedPlayerInventoryWidget->AssignCurrentlyFocusedSlot(this);
		CachedPlayerInventoryWidget->ScrollToSlot(this);

		if (IGamepadControls* GamePadControls = Cast<IGamepadControls>(GetOwningPlayer()))
			GamePadControls->SetCurrentlyFocusedWidget(EWidgetType::Inventory);

		//TODO Delay 0.1f

		RefreshTooltipGamepad();
	}
}

void UPlayerInventorySlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	
	if (IGamepadControls* GamePadControls = Cast<IGamepadControls>(GetOwningPlayer()))
		GamePadControls->SetCurrentlyFocusedWidget(EWidgetType::None);

	ButtonItem->SetToolTip(nullptr);
	BorderObject->SetBrushColor(BorderUnHovered);
	HideItemComparison();
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
}

void UPlayerInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (IsValid(CachedReceiverInventory))
			CachedReceiverInventory->SwitchedActivePanel(CurrentItemData->Class.GetDefaultObject()->InventoryPanel);

		UItemDataDragAndDropPanel* DragAndDropPanel = CreateWidget<UItemDataDragAndDropPanel>(this, ItemDataDragAndDropPanel);
		DragAndDropPanel->InitializePanel(CurrentItemData->Class.GetDefaultObject()->ImageItem, ImageSize);

		auto Operation = Cast<UItemDataDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(ItemDataDragDropOperation));
		Operation->DefaultDragVisual = DragAndDropPanel;
		Operation->ItemData = CurrentItemData;
		Operation->Inventory = CachedPlayerInventory;
		Operation->ItemDataDragAndDropPanel = DragAndDropPanel;
		Operation->DraggerFrom = EItemDestination::InventorySlot;

		OutOperation = Operation;
	}
}

void UPlayerInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		Operation->ClearDraggableIcon();
		BorderObject->SetBrushColor(BorderUnHovered);
	}
}

bool UPlayerInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	FItemData* ItemData = CurrentItemData;
	UItemDataDragDropOperation* DragOperation = Cast<UItemDataDragDropOperation>(InOperation);

	if (InDragDropEvent.IsShiftDown())
	{
		bool Result = AttemptSplitting(DragOperation);

		if (Result)
		{
			BorderObject->SetBrushColor(BorderUnHovered);
			return true;
		}
	}

	switch (DragOperation->DraggerFrom)
	{
	case EItemDestination::InventorySlot:
		DraggedFromInventory(DragOperation);
		break;
	case EItemDestination::EquipmentSlot:
		DraggedFromEquipment(DragOperation, ItemData);
		break;
	case EItemDestination::VendorSlot:
	case EItemDestination::StorageSlot:	
		DraggedFromOtherInventory(DragOperation);
		break;
	default: ;
	}

	return false;
}

bool UPlayerInventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                            UDragDropOperation* InOperation)
{
	if (auto Operation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		 FItemData* DraggedData = Operation->ItemData;
		
		 if (InventoryUtility::IsItemClassValid(CurrentItemData))
		 {
			switch (Operation->DraggerFrom)
			{
			case EItemDestination::InventorySlot:
				return ShowSwapSlotAndGreenBorderColor(Operation);
			case EItemDestination::EquipmentSlot:
				{
					if (InventoryUtility::AreItemsTheSame(CurrentItemData, DraggedData))
					{
						if (CachedPlayerEquipment->CanItemBeEquipped(CurrentItemData).Get<0>())
							return ShowSwapSlotAndGreenBorderColor(Operation);
						return ShowDropSlotAndGreenBorderColor(Operation);
					}
					else
					{
						if (CachedPlayerEquipment->bIsEnableOffHand)
						{
							if (InventoryUtility::CanWeaponsBeSwapped(CurrentItemData, Operation->ItemData))
							{
								if (CachedPlayerEquipment->CanItemBeEquipped(CurrentItemData).Get<0>())
									return ShowSwapSlotAndGreenBorderColor(Operation);
							}
		
							return ShowWrongSlotAndRedBorderColor(Operation);
						}
						return ShowWrongSlotAndRedBorderColor(Operation);
					}
				}
			case EItemDestination::VendorSlot:
			case EItemDestination::StorageSlot:
				{
					if (InventoryUtility::AreItemsTheSame(CurrentItemData, DraggedData))
						if (InventoryUtility::AreItemsStackable(CurrentItemData, DraggedData))
							return ShowDropSlotAndGreenBorderColor(Operation);
					return ShowWrongSlotAndRedBorderColor(Operation);
				}
			default: ;
			}
		 }
		 else
		 {
		 	return ShowDropSlotAndGreenBorderColor(Operation);
		 }
	}

	return true;
}

FReply UPlayerInventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this, EKeys::LeftMouseButton).NativeReply;
}

FReply UPlayerInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClick();
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply UPlayerInventorySlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (IsUsingGamepad())
	{
		ButtonItem->SetKeyboardFocus();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UPlayerInventorySlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	KeyInput = InKeyEvent.GetKey();
	if (KeyInput == EKeys::LeftShift || KeyInput == EKeys::Gamepad_FaceButton_Left)
	{
		ShowItemComparison();
		return FReply::Handled();
	}
	if (KeyInput == EKeys::Gamepad_FaceButton_Top)
	{
		OnDropKeyPressed();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UPlayerInventorySlot::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	KeyInput = InKeyEvent.GetKey();
	if (KeyInput == EKeys::LeftShift || KeyInput == EKeys::Gamepad_FaceButton_Left)
	{
		HideItemComparison();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UPlayerInventorySlot::DraggedFromInventory(UItemDataDragDropOperation* Operation) const
{
	if (Operation->ItemData->Index != CurrentItemData->Index)
	{
		CachedPlayerInventory->ServerMoveItemToSlot(CachedPlayerInventory,
													InventoryUtility::GetInventoryPanelFromItem(Operation->ItemData),
													Operation->ItemData->Index, CurrentItemData->Index);
	}
}

void UPlayerInventorySlot::DraggedFromEquipment(UItemDataDragDropOperation* Operation, FItemData* InSlotData) const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (InventoryUtility::AreItemSlotsEqual(Operation->ItemData, CurrentItemData))
		{
			CachedPlayerEquipment->ServerTransferItemFromInventory(CurrentItemData, Operation->ItemData,CachedPlayerInventory,EInputMethodType::DragAndDrop);
			InventoryUtility::PlaySoundOnItemDropped();
		}
		else
		{	
			if (CachedPlayerEquipment->bIsEnableOffHand)
			{
				if (InventoryUtility::CanWeaponsBeSwapped(CurrentItemData, Operation->ItemData))
				{
					CachedPlayerEquipment->ServerTransferItemFromInventory(CurrentItemData, Operation->ItemData, CachedPlayerInventory, EInputMethodType::DragAndDrop);
					InventoryUtility::PlaySoundOnItemDropped();
				}
			}
		}
	}
	else
	{
		CachedPlayerInventory->ServerTransferItemFromEquipment(Operation->ItemData, InSlotData,
		                                                       EInputMethodType::DragAndDrop,
		                                                       CachedPlayerEquipment);
	}
}


void UPlayerInventorySlot::DraggedFromOtherInventory(UItemDataDragDropOperation* Operation) const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (InventoryUtility::AreItemsTheSame(Operation->ItemData, CurrentItemData))
		{
			if (InventoryUtility::AreItemsStackable(Operation->ItemData, CurrentItemData))
			{
				OnEndDraggedFromOtherInventory(Operation);
			}
		}
	}
	else
		OnEndDraggedFromOtherInventory(Operation);
}

void UPlayerInventorySlot::OnEndDraggedFromOtherInventory(UItemDataDragDropOperation* Operation) const
{
	//TODO Метод создан чтобы не дублировать код!
	if (InventoryUtility::IsStackableAndHaveStacks(Operation->ItemData, 1))
	{
		if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
			WidgetManager->OpenSplitStackPopup(Operation->ItemData, CurrentItemData, Operation->Inventory,
			                                   CachedPlayerInventory,
			                                   EInputMethodType::DragAndDrop, Operation->DraggerFrom,
			                                   EItemDestination::InventorySlot, nullptr);

		BorderObject->SetBrushColor(BorderUnHovered);
	}
	else
	{
		CachedPlayerInventory->ServerTransferItemFromInventory(CachedPlayerInventory, Operation->ItemData,
		                                                       CurrentItemData,
		                                                       EInputMethodType::DragAndDrop, Operation->Inventory,
		                                                       GetOwningPlayer());
	}
}

bool UPlayerInventorySlot::ShowDropSlotAndGreenBorderColor(UItemDataDragDropOperation* Operation) const
{
	Operation->ShowIconDrop();
	BorderObject->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
	return true;
}

bool UPlayerInventorySlot::ShowWrongSlotAndRedBorderColor(UItemDataDragDropOperation* Operation) const
{
	Operation->ShowIconWrongSlot();
	BorderObject->SetBrushColor(FLinearColor(0.737911f, 0.0f, 0.028426f));
	return true;
}

bool UPlayerInventorySlot::ShowSwapSlotAndGreenBorderColor(UItemDataDragDropOperation* Operation) const
{
	Operation->ShowIconSwap();
	BorderObject->SetBrushColor(FLinearColor(0.116971f, 0.48515f, 0.08022f));
	return true;
}


void UPlayerInventorySlot::OnClickedButtonItem()
{
	if(!bIsInitialInputDelay)
		OnRightClick();
}

void UPlayerInventorySlot::OnDropKeyPressed() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
		if (GetActiveWidgetType() == EWidgetType::Inventory)
			DropOnTheGround();
}

void UPlayerInventorySlot::OnHovered()
{
	if (!IsAnyPopUpActive() && !IsUsingGamepad())
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

void UPlayerInventorySlot::OnUnhovered()
{
	if (IGamepadControls* GamePadControls = Cast<IGamepadControls>(GetOwningPlayer()))
		GamePadControls->SetCurrentlyFocusedWidget(EWidgetType::None);

	ButtonItem->SetToolTip(nullptr);
	BorderObject->SetBrushColor(BorderUnHovered);
	HideItemComparison();
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
}

void UPlayerInventorySlot::ShowItemComparison() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
		if (CurrentItemData->Class.GetDefaultObject()->Rarity != EItemRarity::Consumable)
			 if (IsValid(CachedToolTip))
			 	CachedToolTip->ShowComparisonToolTip();
}

void UPlayerInventorySlot::HideItemComparison() const
{
	if (IsValid(CachedToolTip))
		CachedToolTip->HideComparisonToolTip();
}

void UPlayerInventorySlot::SetToolTipPositionAndAlignment() const
{
	FVector2D HorizontalFirstPixelPosition;
	FVector2D HorizontalFirstViewportPosition;
	FVector2D HorizontalSecondPixelPosition;
	FVector2D HorizontalSecondViewportPosition;
	
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),GetCachedGeometry().GetLocalSize(),
		HorizontalFirstPixelPosition,HorizontalFirstViewportPosition);
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),FVector2D(0, GetCachedGeometry().GetLocalSize().Y),
	HorizontalSecondPixelPosition, HorizontalSecondViewportPosition);

	 float HorizontalMaxValue = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).X -
	 	(CachedToolTip->GetDesiredSize().X * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

	bool HorizontalValue = UKismetMathLibrary::InRange_FloatFloat(HorizontalSecondPixelPosition.X, 0.0f, HorizontalMaxValue, true, true);

	float HorizontalPosition = HorizontalValue ? HorizontalSecondViewportPosition.X : HorizontalFirstViewportPosition.X;
	float HorizontalAlignment = HorizontalValue ? 0.0f : 1.0f;
	

	FVector2D VerticalFirstPixelPosition;
	FVector2D VerticalFirstViewportPosition;
	FVector2D VerticalSecondPixelPosition;
	FVector2D VerticalSecondViewportPosition;
	
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),FVector2D(GetCachedGeometry().GetLocalSize().X,0),
	VerticalFirstPixelPosition,VerticalFirstViewportPosition);
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),GetCachedGeometry().GetLocalSize(),
	VerticalSecondPixelPosition, VerticalSecondViewportPosition);

	float VerticalMaxValue = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).Y -
		(CachedToolTip->GetDesiredSize().Y * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

	bool VerticalValue = UKismetMathLibrary::InRange_FloatFloat(VerticalSecondPixelPosition.Y, 0.0f, VerticalMaxValue, true, true);

	float VerticalPosition = VerticalValue ? VerticalSecondViewportPosition.Y : VerticalFirstViewportPosition.Y;
	float VerticalAlignment = VerticalValue ? 0.0f : 1.0f;

	 CachedToolTip->SetPositionInViewport(FVector2D(HorizontalPosition, VerticalPosition), false);
	 CachedToolTip->SetAlignmentInViewport(FVector2D(HorizontalAlignment, VerticalAlignment));
	 CachedToolTip->SetDesiredSizeInViewport(CachedToolTip->GetDesiredSize());
}

void UPlayerInventorySlot::RefreshTooltipGamepad()
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

void UPlayerInventorySlot::RefreshToolTip()
{
	if (IsUsingGamepad())
	{
		if (HasUserFocusedDescendants(GetOwningPlayer()))
		{
			ButtonItem->SetToolTip(nullptr);
			HideItemComparison();
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
			ButtonItem->SetToolTip(nullptr);
			HideItemComparison();
			if (IsValid(CachedToolTip))
				CachedToolTip->RemoveFromParent();
			
			CachedToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
			CachedToolTip->InitializeToolTip(CurrentItemData, false);
			ButtonItem->SetToolTip(CachedToolTip);
		}
	}
}

void UPlayerInventorySlot::DropOnTheGround() const
{
	if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
	{
		if (CachedPlayerInventory->GetItemRemoveType(CurrentItemData) == EItemRemoveType::OnConfirmation)
			WidgetManager->DisplayMessageNotify("Item cannot be Removed.");

		WidgetManager->OpenConfirmationPopup("Are you sure you want to remove?", CurrentItemData, nullptr, nullptr,
		                                     CachedPlayerInventory, EInputMethodType::RightClick,
		                                     EItemDestination::InventorySlot, EItemDestination::DropBar,
		                                     CachedPlayerInventoryWidget);

		if (InventoryUtility::IsStackableAndHaveStacks(CurrentItemData, 1))
		{
			WidgetManager->OpenSplitStackPopup(CurrentItemData, nullptr, nullptr, CachedPlayerInventory,
			                                   EInputMethodType::RightClick, EItemDestination::InventorySlot,
			                                   EItemDestination::DropBar, CachedPlayerInventoryWidget);
		}
		else
			CachedPlayerInventory->ServerDropItemOnTheGround(CurrentItemData, EItemDestination::InventorySlot,
			                                                 GetOwningPlayer());
	}
}

void UPlayerInventorySlot::OnRightClick()
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
		{
			switch (WidgetManager->GetActiveWidget())
			{
			case EWidgetType::Inventory:
			case EWidgetType::Equipment:
				OpenInventoryWindow();
				break;
			case EWidgetType::Vendor:
			case EWidgetType::Storage:
				
				if (WidgetManager->GetActiveTab() == EWidgetType::Equipment)
					OpenInventoryWindow();
				else
					OpenVendorStorageWindow();
				break;
			default: ;
			}
		}
	}
}

void UPlayerInventorySlot::EmptySlot()
{
	CurrentItemData = new FItemData();
	CurrentItemData->Index = SlotIndex;

	SetButtonStyle(CurrentItemData);
	SetItemQuantity();
	ButtonItem->SetToolTip(nullptr);
	if (IsValid(CachedToolTip))
		CachedToolTip->RemoveFromParent();
}

void UPlayerInventorySlot::SetItemQuantity() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (CurrentItemData->Class.GetDefaultObject()->bIsStackable && CurrentItemData->Quantity > 1)
			TextQuantity->SetText(UKismetTextLibrary::Conv_IntToText(CurrentItemData->Quantity));
		else
			TextQuantity->SetText(FText());
	}
	else
		TextQuantity->SetText(FText());
}


void UPlayerInventorySlot::OverwriteSlot(FItemData* ItemData)
{	
	CurrentItemData = ItemData;
	SetButtonStyle(CurrentItemData);
	SetItemQuantity();
	RefreshToolTip();
}

void UPlayerInventorySlot::SetButtonStyle(FItemData* ItemData) const
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

EWidgetType UPlayerInventorySlot::GetActiveWidgetType() const
{
	if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
		return WidgetManager->GetCurrentPopupType();
	return EWidgetType::None;
}


bool UPlayerInventorySlot::AttemptSplitting(UItemDataDragDropOperation* Operation) const
{
	if (Operation->DraggerFrom == EItemDestination::InventorySlot && Operation->ItemData->Index != CurrentItemData->
		Index)
		return false;

	if (Operation->DraggerFrom == EItemDestination::InventorySlot ||
		Operation->DraggerFrom == EItemDestination::EquipmentSlot)
	{
		if (InventoryUtility::IsItemClassValid(CurrentItemData))
		{
			if (InventoryUtility::AreItemsTheSame(Operation->ItemData, CurrentItemData))
			{
				if (InventoryUtility::IsStackableAndHaveStacks(Operation->ItemData, 1))
				{
					if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
					{
						WidgetManager->OpenSplitStackPopup(Operation->ItemData, CurrentItemData,
						                                   nullptr, CachedPlayerInventory,
						                                   EInputMethodType::DragAndDrop,
						                                   Operation->DraggerFrom, EItemDestination::InventorySlot,
						                                   nullptr);
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool UPlayerInventorySlot::IsUsingGamepad() const
{
	if (IsValid(CachedPlayerController))
		return CachedPlayerController->IsUsingGamepad();
	return false;
}

bool UPlayerInventorySlot::IsAnyPopUpActive() const
{
	if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
		return WidgetManager->GetCurrentPopupType() != EWidgetType::None;

	return false;
}

void UPlayerInventorySlot::OpenInventoryWindow() const
{
	if(CurrentItemData->EquipmentSlot != EItemSlot::None)
	{
		CachedPlayerEquipment->ServerTransferItemFromInventory(CurrentItemData, new FItemData(),
			CachedPlayerInventory, EInputMethodType::RightClick);
	}
	else
		TryToUseAnItem();
}

void UPlayerInventorySlot::OpenVendorStorageWindow() const
{
	switch (CachedPlayerInventory->GetItemRemoveType(CurrentItemData))
	{
	case EItemRemoveType::Default:
		if (InventoryUtility::IsStackableAndHaveStacks(CurrentItemData, 1))
		{
			if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
			{
				WidgetManager->OpenSplitStackPopup(CurrentItemData, nullptr, CachedPlayerInventory,
				                                   CachedReceiverInventory,
				                                   EInputMethodType::RightClick, EItemDestination::InventorySlot,
				                                   EItemDestination::VendorSlot,
				                                   CachedPlayerInventoryWidget);
			}
		}
		else
		{
			CachedPlayerInventory->ServerTransferItemFromInventory(CachedReceiverInventory, CurrentItemData,
			                                                       nullptr, EInputMethodType::RightClick,
			                                                       CachedPlayerInventory, GetOwningPlayer());
		}
		break;
	case EItemRemoveType::OnConfirmation:
		if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
		{
			WidgetManager->OpenConfirmationPopup("Are you sure you want to remove?", CurrentItemData, nullptr,
			                                     CachedPlayerInventory,
			                                     CachedReceiverInventory, EInputMethodType::RightClick,
			                                     EItemDestination::InventorySlot,
			                                     EItemDestination::VendorSlot, CachedPlayerInventoryWidget);
		}	
		break;
	case EItemRemoveType::CannotBeRemoved:
		if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
			WidgetManager->DisplayMessageNotify("Item cannot be Removed.");
		break;
	}
}

void UPlayerInventorySlot::TryToUseAnItem() const
{
	FItemData* ItemData = CurrentItemData;
	if(ItemData->Class.GetDefaultObject()->UseType ==  EItemUseType::TextDocument)
	{
		ItemData->bIsAlreadyUsed = true;
		CachedPlayerInventory->ServerAddItemToInventory(CachedPlayerInventory, ItemData, ItemData->Index);
		if (auto WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager())
			WidgetManager->OpenTextDocumentPopup(ItemData, CachedPlayerInventoryWidget);
	}
}
