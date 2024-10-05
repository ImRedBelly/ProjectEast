#include "WidgetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/UI/Crafting/ForgeStationWindow.h"
#include "ProjectEast/Core/UI/Crafting/PlayerCraftingWindow.h"
#include "ProjectEast/Core/UI/HUD/MainWindow.h"
#include "ProjectEast/Core/UI/Menus/Pause.h"
#include "ProjectEast/Core/UI/Misc/Popups/PopupMessage.h"
#include "ProjectEast/Core/UI/PlayerInventory/InventoryWindow.h"
#include "ProjectEast/Core/UI/PlayerInventory/SplitStackPopup.h"
#include "ProjectEast/Core/UI/Storage/StorageWindow.h"
#include "ProjectEast/Core/UI/Vendor/VendorWindow.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UWidgetManager::InitializeWidgetManager(AMainPlayerController* InPlayerController, APlayerCapture* InPlayerCapture)
{
	CachedMainWindow = CreateWidget<UMainWindow>(InPlayerController, DefaultMainWindow);
	if (UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
		CachedMainWindow->AddToPlayerScreen();
	else
		CachedMainWindow->AddToViewport();

	
	CachedPlayerController = InPlayerController;
	CachedPlayerCapture = InPlayerCapture;
	CachedPlayerInventory = InPlayerController->GetPlayerInventory();
	

}

void UWidgetManager::SetActiveWidget(EWidgetType WidgetType)
{
	ActiveWidget = WidgetType;
	if (OnSwitchWidget.IsBound())
		OnSwitchWidget.Broadcast(ActiveWidget);
}

EWidgetType UWidgetManager::GetActiveWidget()
{
	return ActiveWidget;
}

void UWidgetManager::SetActiveTab(EWidgetType WidgetType)
{
	ActiveTab = WidgetType;
	if (OnSwitchTab.IsBound())
		OnSwitchTab.Broadcast(ActiveTab);
}

EWidgetType UWidgetManager::GetActiveTab()
{
	return ActiveTab;
}

void UWidgetManager::OpenNewWidget(EWidgetType WidgetType)
{
	if (WidgetType == ActiveWidget)
		return;

	FInputModeGameAndUI InputMode;
	switch (WidgetType)
	{
	case EWidgetType::Pause:
		{
			CashedPause = CreateWidget<UPause>(CachedPlayerController, DefaultPauseWindow);
			if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
				CashedPause->AddToViewport(1);
			else
				CashedPause->AddToPlayerScreen(1);

			InputMode.SetWidgetToFocus(CashedPause->TakeWidget());
		}
		break;
	case EWidgetType::Inventory:
		{
			SetActiveTab(EWidgetType::Inventory);
			StartPlayerCapture();

			CashedInventoryWindow = CreateWidget<UInventoryWindow>(CachedPlayerController, DefaultInventoryWindow);
			if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
				CashedInventoryWindow->AddToViewport(1);
			else
				CashedInventoryWindow->AddToPlayerScreen(1);

			InputMode.SetWidgetToFocus(CashedInventoryWindow->TakeWidget());
		}
		break;
	case EWidgetType::Storage:
		{
			InventoryUtility::PlaySoundOnOpeningWidget();
			SetActiveTab(EWidgetType::Storage);
			StartPlayerCapture();


			CashedStorageWindow = CreateWidget<UStorageWindow>(CachedPlayerController, DefaultStorageWindow);
			if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
				CashedStorageWindow->AddToViewport(1);
			else
				CashedStorageWindow->AddToPlayerScreen(1);
			InputMode.SetWidgetToFocus(CashedStorageWindow->TakeWidget());
		}
		CachedPlayerInventory->OpenStorageWidget();
		break;
	case EWidgetType::Vendor:
		{
			InventoryUtility::PlaySoundOnOpeningWidget();
			SetActiveTab(EWidgetType::Vendor);
			StartPlayerCapture();

			CachedVendorWindow = CreateWidget<UVendorWindow>(CachedPlayerController, VendorWindowClass);

			if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
				CachedVendorWindow->AddToViewport(1);
			else
				CachedVendorWindow->AddToPlayerScreen(1);

			CachedVendorWindow->InitializeWindow(CachedPlayerController->GetPlayerInventory(), this);
			InputMode.SetWidgetToFocus(CachedVendorWindow->TakeWidget());
		}
		break;
	case EWidgetType::PlayerCrafting:
		{
			InventoryUtility::PlaySoundOnOpeningWidget();
			SetActiveTab(EWidgetType::PlayerCrafting);
			StartPlayerCapture();

			CachedPlayerCraftingWindow = CreateWidget<
				UCraftingWindowCore>(CachedPlayerController, PlayerCraftingWindow);

			if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
				CachedPlayerCraftingWindow->AddToViewport(1);
			else
				CachedPlayerCraftingWindow->AddToPlayerScreen(1);

			CachedPlayerCraftingWindow->InitializeWindow(CachedPlayerController->GetPlayerCrafting(), CraftingStation);
			InputMode.SetWidgetToFocus(CachedPlayerCraftingWindow->TakeWidget());
		}
		break;
	case EWidgetType::StationCrafting:
		{
			InventoryUtility::PlaySoundOnOpeningWidget();
			SetActiveTab(EWidgetType::StationCrafting);
			StartPlayerCapture();

			CachedStationCraftingWindow = CreateWidget<UCraftingWindowCore>(
				CachedPlayerController, StationCraftingWindow);

			if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
				CachedStationCraftingWindow->AddToViewport(1);
			else
				CachedStationCraftingWindow->AddToPlayerScreen(1);

			CachedStationCraftingWindow->InitializeWindow(CachedPlayerController->GetPlayerCrafting(), CraftingStation);
			InputMode.SetWidgetToFocus(CachedStationCraftingWindow->TakeWidget());
		}
		break;
	default: ;
	}

	SetActiveWidget(WidgetType);

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	CachedPlayerController->SetInputMode(InputMode);
	CachedPlayerController->SetShowMouseCursor(true);
	CachedPlayerController->SetIgnoreMoveInput(true);
	CachedPlayerController->SetIgnoreLookInput(true);
}

void UWidgetManager::SwitchWidgetTo(EWidgetType WidgetType)
{
	if (WidgetType != ActiveWidget)
	{
		CloseActiveWidget();
		OpenNewWidget(WidgetType);
	}
}

void UWidgetManager::SwitchTabTo(EWidgetType WidgetType)
{
	SetActiveTab(WidgetType);
}

void UWidgetManager::CloseActiveWidget()
{
	switch (ActiveWidget)
	{
	case EWidgetType::Pause:
		{
			SetActiveWidget(EWidgetType::None);
			SetActiveTab(EWidgetType::None);

			if (IsValid(CashedPause))
				CashedPause->RemoveFromParent();
		}
		break;
	case EWidgetType::Inventory:
		{
			SetActiveWidget(EWidgetType::None);
			SetActiveTab(EWidgetType::None);
			StopPlayerCapture();

			if (IsValid(CashedInventoryWindow))
				CashedInventoryWindow->RemoveFromParent();
		}
		break;
	case EWidgetType::Storage:
		{
			CachedPlayerInventory->CloseStorageWidget();
			SetActiveWidget(EWidgetType::None);
			SetActiveTab(EWidgetType::None);
			StopPlayerCapture();

			if (IsValid(CashedStorageWindow))
				CashedStorageWindow->RemoveFromParent();
		}
		break;
	case EWidgetType::PlayerCrafting:
		{
			SetActiveWidget(EWidgetType::None);
			SetActiveTab(EWidgetType::None);
			StopPlayerCapture();

			if (IsValid(CachedPlayerCraftingWindow))
				CachedPlayerCraftingWindow->RemoveFromParent();
		}
		break;
	case EWidgetType::Vendor:
		{
			SetActiveWidget(EWidgetType::None);
			SetActiveTab(EWidgetType::None);
			StopPlayerCapture();

			if (IsValid(CachedVendorWindow))
			{
				CachedVendorWindow->RemoveFromParent();
				CachedVendorWindow = nullptr;
			}
		}
		break;
	case EWidgetType::StationCrafting:
		{
			SetActiveWidget(EWidgetType::None);
			SetActiveTab(EWidgetType::None);
			StopPlayerCapture();

			if (IsValid(CachedStationCraftingWindow))
				CachedStationCraftingWindow->RemoveFromParent();
		}
		break;
	default: ;
	}


	FInputModeGameOnly InputMode;
	CachedPlayerController->SetInputMode(InputMode);
	CachedPlayerController->SetShowMouseCursor(false);
	CachedPlayerController->SetIgnoreMoveInput(false);
	CachedPlayerController->SetIgnoreLookInput(false);

	CachedPlayerInventory->ServerChangeDroppedIndex(0);
}

void UWidgetManager::CloseActivePopup()
{
	ActivePopup = EWidgetPopup::None;
	if (IsValid(CachedSplitStackPopup))
		CachedSplitStackPopup->RemoveFromParent();
}

EWidgetType UWidgetManager::GetActivePopup()
{
	return EWidgetType::Inventory;
}

EWidgetType UWidgetManager::GetCurrentPopupType()
{
	return EWidgetType::None;
}


void UWidgetManager::StartPlayerCapture()
{
	CachedPlayerCapture->StartCapture();
}

void UWidgetManager::StopPlayerCapture()
{
	CachedPlayerCapture->EndCapture();
}

EWidgetType UWidgetManager::GetCurrentlyFocusedWidget()
{
	return EWidgetType::None;
}

void UWidgetManager::SetCurrentlyFocusedWidget(EWidgetType None)
{
}

APlayerCapture* UWidgetManager::GetPlayerCapture() const
{
	return CachedPlayerCapture;
}

UMainWindow* UWidgetManager::GetMainWindow() const
{
	return CachedMainWindow;
}

void UWidgetManager::OpenSplitStackPopup(FItemData* ItemData, FItemData* InSlotData, UInventoryCore* Sender,
                                         UInventoryCore* Receiver, EInputMethodType InputMethod,
                                         EItemDestination Initiator, EItemDestination Destination,
                                         UUserWidget* SenderWidget)
{
	CachedSplitStackPopup = CreateWidget<USplitStackPopup>(CachedPlayerController, DefaultSplitStackPopup);
	CachedSplitStackPopup->InitializePopup(ItemData, InSlotData, Sender, Receiver, InputMethod, Initiator, Destination,
	                                       SenderWidget);

	if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
		CachedSplitStackPopup->AddToViewport(1);
	else
		CachedSplitStackPopup->AddToPlayerScreen(1);

	CachedSplitStackPopup->SetKeyboardFocus();
	ActivePopup = EWidgetPopup::SplitStackPopup;
}

void UWidgetManager::OpenConfirmationPopup(const FString Str, FItemData* ItemData, const FItemData* ItemData1,
                                           const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory,
                                           EInputMethodType RightClick,
                                           EItemDestination InventorySlot, EItemDestination DropBar,
                                           UUserWidget* UserWidget)
{
}

void UWidgetManager::OpenTextDocumentPopup(FItemData* ItemData, UUserWidget* ParentWidget)
{
	CachedSplitStackPopup = CreateWidget<USplitStackPopup>(CachedPlayerController, DefaultSplitStackPopup);
}

void UWidgetManager::DisplayMessage(const FString Message)
{
	if (!Message.IsEmpty())
	{
		auto NewPopupMessage = CreateWidget<UPopupMessage>(CachedPlayerController, PopupMessageClass);
		NewPopupMessage->DisplayMessage(FText::FromString(Message));

		if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
			NewPopupMessage->AddToViewport(1);
		else
			NewPopupMessage->AddToPlayerScreen(1);
	}
}

void UWidgetManager::InitializeCraftingStation(UCraftingCore* CraftingCore)
{
	CraftingStation = CraftingCore;
}

bool UWidgetManager::IsAnyMainWidgetOpen() const
{
	return ActiveWidget != EWidgetType::None;
}
