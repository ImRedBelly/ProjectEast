#include "StorageWindow.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/UI/PlayerInventory/PlayerInventoryWidget.h"
#include "ProjectEast/Core/UI/PlayerInventory/SortWindow.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UStorageWindow::NativeConstruct()
{
	Super::NativeConstruct();
	ForceLayoutPrepass();
	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	CachedPlayerInventory = CachedPlayerController->GetPlayerInventory();
	WidgetManager = CachedPlayerController->GetWidgetManager();

	BindEventDispatchers();
	PlayAnimation(AnimationConstruct, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UStorageWindow::NativeDestruct()
{
	UnbindEventDispatchers();
	Super::NativeDestruct();
}

FReply UStorageWindow::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	PlayerInventoryPanel->SetKeyboardFocus();
	return FReply::Handled();
}

FReply UStorageWindow::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_RightShoulder)
	{
		WidgetManager->SwitchTabTo(EWidgetType::Equipment);

		if (WidgetManager->GetCurrentlyFocusedWidget() != EWidgetType::Inventory)
			PlayerInventoryPanel->SetFocusToSlot(0);

		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Gamepad_LeftShoulder)
	{
		WidgetManager->SwitchTabTo(EWidgetType::Equipment);

		if (WidgetManager->GetCurrentlyFocusedWidget() != EWidgetType::Storage)
			PlayerInventoryPanel->SetFocusToSlot(0);

		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Gamepad_RightTrigger || InKeyEvent.GetKey() == EKeys::Gamepad_LeftTrigger)
	{
		PlayerInventoryPanel->SwitchToNextInventoryPanel(InKeyEvent.GetKey() == EKeys::Gamepad_RightTrigger);
		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Gamepad_LeftThumbstick)
	{
		PlayerInventoryPanel->GetSortWindow()->ToggleSortingWindow();
		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::F)
	{
		CachedPlayerInventory->ServerSortInventory(CachedPlayerInventory, ESortMethod::Quicksort,
		                                           CachedPlayerInventory->GetActivePanel(), false);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UStorageWindow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

FReply UStorageWindow::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto CursorDelta = InMouseEvent.GetCursorDelta();
	if (CursorDelta.X != 0.0f || CursorDelta.Y != 0.0f)
		return FReply::Handled();
	return FReply::Unhandled();
}

void UStorageWindow::BindEventDispatchers()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer()))
		PlayerController->OnSwitchedTab.AddDynamic(this, &UStorageWindow::SwitchTab);
}

void UStorageWindow::UnbindEventDispatchers()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer()))
		PlayerController->OnSwitchedTab.RemoveDynamic(this, &UStorageWindow::SwitchTab);
}

void UStorageWindow::SwitchTab(EWidgetType WidgetType)
{
	UUserWidget* CurrentWidget = nullptr;
	switch (WidgetType)
	{
	case EWidgetType::None:
	case EWidgetType::Inventory:
	case EWidgetType::StationCrafting:
	case EWidgetType::Vendor:
	case EWidgetType::LoadGame:
	case EWidgetType::Abilities:
		CurrentWidget = nullptr;
		break;
	case EWidgetType::Equipment:
		CurrentWidget = EquipmentPanel;
		break;
	case EWidgetType::Storage:
		CurrentWidget = StoragePanel;
		break;
	}

	if (IsValid(CurrentWidget))
	{
		WidgetSwitcher->SetActiveWidget(CurrentWidget);
		InventoryUtility::PlaySoundOnTabSwitched();
	}
}
