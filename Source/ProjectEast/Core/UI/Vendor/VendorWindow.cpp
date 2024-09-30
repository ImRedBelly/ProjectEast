#include "VendorWindow.h"

#include "VendorInventory.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/UI/Equipment/EquipmentPanel.h"
#include "ProjectEast/Core/UI/PlayerInventory/PlayerInventoryWidget.h"
#include "ProjectEast/Core/UI/PlayerInventory/SortWindow.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UVendorWindow::InitializeWindow(UPlayerInventory* InPlayerInventory, UWidgetManager* InWidgetManager)
{
	PlayerInventory = InPlayerInventory;
	WidgetManager = InWidgetManager;

	ForceLayoutPrepass();
	WidgetManager->OnSwitchTab.AddDynamic(this, &UVendorWindow::SwitchTab);
	VendorInventory->InitializeVendorInventory(PlayerInventory, WidgetManager);
	PlayAnimation(ConstructAnimation);
}

void UVendorWindow::NativeDestruct()
{
	if (IsValid(WidgetManager))
		WidgetManager->OnSwitchTab.RemoveDynamic(this, &UVendorWindow::SwitchTab);
	Super::NativeDestruct();
}

FReply UVendorWindow::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (IsValid(WidgetManager))
		return FReply::Unhandled();
	if (InKeyEvent.GetKey() == EKeys::Gamepad_RightShoulder)
	{
		WidgetManager->SwitchTabTo(EWidgetType::Equipment);
		if (WidgetManager->GetCurrentlyFocusedWidget() != EWidgetType::Inventory)
			PlayerInventoryWidget->SetFocusToSlot(0);

		FReply::Handled();
	}
	else
	{
		if (InKeyEvent.GetKey() == EKeys::Gamepad_LeftShoulder)
		{
			WidgetManager->SwitchTabTo(EWidgetType::Vendor);
			if (WidgetManager->GetCurrentlyFocusedWidget() != EWidgetType::Inventory)
				PlayerInventoryWidget->SetFocusToSlot(0);

			FReply::Handled();
		}
		else
		{
			if (InKeyEvent.GetKey() == EKeys::Gamepad_RightTrigger ||
				InKeyEvent.GetKey() == EKeys::Gamepad_LeftTrigger)
			{
				PlayerInventoryWidget->SwitchToNextInventoryPanel(InKeyEvent.GetKey() == EKeys::Gamepad_RightTrigger);
				FReply::Handled();
			}
			else
			{
				if (InKeyEvent.GetKey() == EKeys::Gamepad_LeftThumbstick)
				{
					PlayerInventoryWidget->GetSortWindow()->ToggleSortingWindow();
					FReply::Handled();
				}
				else
				{
					if (InKeyEvent.GetKey() == EKeys::F)
					{
						PlayerInventory->ServerSortInventory(PlayerInventory, ESortMethod::Quicksort,
						                                     PlayerInventory->GetActivePanel(), false);
						FReply::Handled();
					}
				}
			}
		}
	}
	return FReply::Unhandled();
}

FReply UVendorWindow::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	PlayerInventoryWidget->SetKeyboardFocus();
	return FReply::Handled();
}

FReply UVendorWindow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void UVendorWindow::SwitchTab(EWidgetType WidgetType)
{
	switch (WidgetType)
	{
	case EWidgetType::Equipment:
		{
			WidgetSwitcher->SetActiveWidget(EquipmentPanel);
			InventoryUtility::PlaySoundOnTabSwitched();
		}
		break;
	case EWidgetType::Vendor:
		{
			WidgetSwitcher->SetActiveWidget(VendorInventory);
			InventoryUtility::PlaySoundOnTabSwitched();
		}
		break;
	default: ;
	}
}