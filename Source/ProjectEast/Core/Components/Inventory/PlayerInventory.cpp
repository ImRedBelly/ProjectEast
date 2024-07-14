#include "PlayerInventory.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Actors/Interfaces/ObjectInteraction.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"


void UPlayerInventory::BeginPlay()
{
}

void UPlayerInventory::ClientTransferItemReturnValue(bool Success, FText FailureMessage)
{
	Super::ClientTransferItemReturnValue(Success, FailureMessage);
}

void UPlayerInventory::InitializeInventory(APlayerController* PlayerController)
{
	ClientInitializeInventory(PlayerController);
	Super::InitializeInventory(PlayerController);
}

void UPlayerInventory::ClientInitializeInventory(APlayerController* PlayerController)
{
	CashedPlayerController = Cast<AMainPlayerController>(PlayerController);
	if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		for (auto Panel : PanelsToUse)
			BuildInventory(Panel);
	}
}

void UPlayerInventory::ServerTakeItem(FItemData* ItemData, UInventoryCore* Sender, AActor* OwningPlayer)
{
}

void UPlayerInventory::ServerTakeAllItems(UInventoryCore* Sender, AActor* OwningPlayer)
{
}

void UPlayerInventory::ServerSpawnLootBag(FItemData* ItemData, AActor* OwningPlayer)
{
}

void UPlayerInventory::ServerSpawnInteractable(FItemData* ItemData, AActor* OwningPlayer)
{
}

void UPlayerInventory::ServerDropItemOnTheGround(FItemData* ItemData, EItemDestination Initiator, AActor* OwningPlayer)
{
}

void UPlayerInventory::ServerChangeDroppedIndex(uint32 DroppedIndex)
{
}

void UPlayerInventory::ServerModifyItemDurability(FItemData* ItemData, uint32 Amount, AActor* OwningPlayer)
{
}

void UPlayerInventory::ClientItemLooted(FItemData* ItemData)
{
}

void UPlayerInventory::ClientTakeItemReturnValue(bool Success, FText FailureMessage, bool RemoveInteraction)
{
}

void UPlayerInventory::OpenInventoryWidget()
{
	if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(CashedPlayerController))
	{
		WidgetManager->SetActiveWidget(EWidgetType::Inventory);
		WidgetManager->SetActiveTab(EWidgetType::Inventory);
		WidgetManager->StartPlayerCapture();
	}

	CashedInventoryWindow = CreateWidget<UInventoryWindow>(CashedPlayerController, DefaultInventoryWindow);
	if(!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
		CashedInventoryWindow->AddToViewport(1);
	else
		CashedInventoryWindow->AddToPlayerScreen(1);
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(CashedInventoryWindow->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	CashedPlayerController->SetInputMode(InputMode);
	CashedPlayerController->SetShowMouseCursor(true);
	CashedPlayerController->SetIgnoreMoveInput(true);
	CashedPlayerController->SetIgnoreLookInput(true);
}

void UPlayerInventory::CloseInventoryWidget()
{
}

void UPlayerInventory::InputCloseWidget()
{
	if (bIsInteractableActorWidgetOpen)
	{
		CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());
	}
	else
	{
		CashedPlayerController->CloseActiveWidget();
	}
}

void UPlayerInventory::InputInteraction() const
{
	if (bIsLootBarOpen)
	{
		if (OnTakeItem.IsBound())
			OnTakeItem.Broadcast();
	}
	else
	{
		if (!bIsInteractableActorWidgetOpen)
		{
			if (IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(CashedPlayerController))
				ObjectInteraction->InitializeInteractionWithObject(
					InventoryUtility::GetCurrentInteractableObject(CashedPlayerController));
		}
	}
}

void UPlayerInventory::InputCloseWidget() const
{
}

void UPlayerInventory::InputInventory()
{
	if(bIsLootBarOpen)
		CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());

	if(CashedPlayerController->GetActiveWidget() == EWidgetType::Inventory)
		CloseInventoryWidget();
	
	else
	{
		if(bIsInteractableActorWidgetOpen)
			CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());
		
		CashedPlayerController->SwitchWidgetTo(EWidgetType::Inventory);
	}
}

void UPlayerInventory::InputTakeAll() const
{
}

UInteractableComponent* UPlayerInventory::GetCurrentInteractable() const
{
	return InventoryUtility::GetCurrentInteractableObject(CashedPlayerController);
}

void UPlayerInventory::OpenLootBarWidget()
{
	InventoryUtility::PlaySoundOnOpeningWidget();
	bIsLootBarOpen = true;
	bIsInteractableActorWidgetOpen = true;

	if (IsValid(CashedPlayerController->GetMainWindow()))
	{
		UMainWindow* MainWindow = CashedPlayerController->GetMainWindow();
		MainWindow->ShowLootBar();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(MainWindow->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);
		CashedPlayerController->SetInputMode(InputMode);
		CashedPlayerController->SetShowMouseCursor(true);
		CashedPlayerController->SetIgnoreMoveInput(true);
		CashedPlayerController->SetIgnoreLookInput(true);
	}
}

void UPlayerInventory::CloseLootBarWidget()
{
	bIsLootBarOpen = false;
	bIsInteractableActorWidgetOpen = false;
	if (IsValid(CashedPlayerController->GetMainWindow()))
	{
		auto MainWindow = CashedPlayerController->GetMainWindow();
		MainWindow->HideLootBar();

		FInputModeGameOnly InputMode;
		CashedPlayerController->SetInputMode(InputMode);

		CashedPlayerController->SetShowMouseCursor(false);
		CashedPlayerController->ResetIgnoreInputFlags();
	}
}

void UPlayerInventory::OpenVendorWidget()
{
}

void UPlayerInventory::CloseVendorWidget()
{
}

void UPlayerInventory::OpenStorageWidget()
{
}

void UPlayerInventory::CloseStorageWidget()
{
}

void UPlayerInventory::TakeItem()
{
}

void UPlayerInventory::TakeAllItems()
{
}

void UPlayerInventory::DropItemOnTheGround()
{
}

void UPlayerInventory::SpawnLootBagNearThePlayer()
{
}

void UPlayerInventory::SpawnItemMeshNearThePlayer()
{
}

void UPlayerInventory::IsCollidingWithLootBag()
{
}

TTuple<bool, FText> UPlayerInventory::TransferItemFromInventory(FItemData* ItemData, FItemData* IsSlotData,
                                                                EInputMethodType InputMethod, UInventoryCore* Inventory,
                                                                AActor* OwningPlayer)
{
	return Super::TransferItemFromInventory(ItemData, IsSlotData, InputMethod, Inventory, OwningPlayer);
}

void UPlayerInventory::SplitItemsInInventory(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
                                             FItemData* StackableLeft, EInputMethodType Method,
                                             EInputMethodType Initiator, EInputMethodType Destination,
                                             AActor* OwningPlayer)
{
	Super::SplitItemsInInventory(Sender, ItemData, InSlotData, StackableLeft, Method, Initiator, Destination,
	                             OwningPlayer);
}

void UPlayerInventory::ConfirmationPopupAccepted(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
                                                 EInputMethodType Method, EInputMethodType Initiator,
                                                 EInputMethodType Destination, AActor* OwningPlayer)
{
	Super::ConfirmationPopupAccepted(Sender, ItemData, InSlotData, Method, Initiator, Destination, OwningPlayer);
}

bool UPlayerInventory::AttemptUsingTransferredItem(FItemData* ItemData, UInventoryCore* Sender)
{
	return true;
}

void UPlayerInventory::AddItemToInventoryArray(FItemData* ItemData, int32 Index)
{
	Super::AddItemToInventoryArray(ItemData, Index);
}

void UPlayerInventory::SwapItemsInInventory(FItemData* FirstItem, FItemData* SecondItem)
{
	Super::SwapItemsInInventory(FirstItem, SecondItem);
}
