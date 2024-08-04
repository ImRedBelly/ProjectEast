#include "PlayerInventory.h"
#include "PlayerEquipment.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Actors/Interfaces/ObjectInteraction.h"
#include "ProjectEast/Core/UI/Storage/StorageWindow.h"


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
	TakeItem(ItemData, Sender, OwningPlayer);
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
	DropItemOnTheGround(ItemData, Initiator, OwningPlayer);
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

void UPlayerInventory::ClientTakeItemReturnValue(bool Success, FText FailureMessage, bool RemoveInteraction) const
{
	if (Success)
	{
		InventoryUtility::PlaySoundOnItemPickedUp();
		if (RemoveInteraction)
			CashedPlayerController->RemoveInteractionFromObject(GetCurrentInteractable());
	}
	else
		CashedPlayerController->DisplayMessageNotify(FailureMessage.ToString());
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
	if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
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
	CashedPlayerController->SetActiveWidget(EWidgetType::None);
	CashedPlayerController->SetActiveTab(EWidgetType::None);
	CashedPlayerController->StopPlayerCapture();

	if (IsValid(CashedInventoryWindow))
		CashedInventoryWindow->RemoveFromParent();


	FInputModeGameOnly InputMode;
	CashedPlayerController->SetInputMode(InputMode);

	CashedPlayerController->SetShowMouseCursor(false);
	CashedPlayerController->SetIgnoreMoveInput(false);
	CashedPlayerController->SetIgnoreLookInput(false);
	ServerChangeDroppedIndex(0);
}

void UPlayerInventory::InputCloseWidget()
{
	if (bIsInteractableActorWidgetOpen)
		CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());
	else
		CashedPlayerController->CloseActiveWidget();
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
				ObjectInteraction->InitializeInteractionWithObject(InventoryUtility::GetCurrentInteractableObject(CashedPlayerController));
		}
	}
}

void UPlayerInventory::InputCloseWidget() const
{
}

void UPlayerInventory::InputInventory()
{
	if (bIsLootBarOpen)
		CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());

	if (CashedPlayerController->GetActiveWidget() == EWidgetType::Inventory)
		CloseInventoryWidget();

	else
	{
		if (bIsInteractableActorWidgetOpen)
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
	InventoryUtility::PlaySoundOnOpeningWidget();
	bIsInteractableActorWidgetOpen = true;
	
	if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(CashedPlayerController))
	{
		WidgetManager->SetActiveWidget(EWidgetType::Storage);
		WidgetManager->SetActiveTab(EWidgetType::Storage);
		WidgetManager->StartPlayerCapture();
	}
	
	CashedStorageWindow = CreateWidget<UStorageWindow>(CashedPlayerController, DefaultStorageWindow);
	if (!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
		CashedStorageWindow->AddToViewport(1);
	else
		CashedStorageWindow->AddToPlayerScreen(1);
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(CashedStorageWindow->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	CashedPlayerController->SetInputMode(InputMode);
	CashedPlayerController->SetShowMouseCursor(true);
	CashedPlayerController->SetIgnoreMoveInput(true);
	CashedPlayerController->SetIgnoreLookInput(true);
}

void UPlayerInventory::CloseStorageWidget()
{
	CashedPlayerController->SetActiveWidget(EWidgetType::None);
	CashedPlayerController->SetActiveTab(EWidgetType::None);
	CashedPlayerController->StopPlayerCapture();
	bIsInteractableActorWidgetOpen = false;

	if (IsValid(CashedStorageWindow))
		CashedStorageWindow->RemoveFromParent();


	FInputModeGameOnly InputMode;
	CashedPlayerController->SetInputMode(InputMode);

	CashedPlayerController->SetShowMouseCursor(false);
	CashedPlayerController->SetIgnoreMoveInput(false);
	CashedPlayerController->SetIgnoreLookInput(false);
	ServerChangeDroppedIndex(0);
}

void UPlayerInventory::TakeItem(FItemData* ItemData, UInventoryCore* Sender, AActor* OwningPlayer)
{
	if (InventoryUtility::IsItemClassValid(ItemData) && IsValid(Sender))
	{
		FItemData* LocalItemData = ItemData;
		auto TransferData = TransferItemFromInventory(LocalItemData, nullptr, EInputMethodType::RightClick, Sender,
		                                              OwningPlayer);

		auto InventoryData = Sender->GetInventoryAndSize(EInventoryPanels::P1);
		
		ClientTakeItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>(),
		                          !InventoryData.Get<0>().IsValidIndex(0));

		if (TransferData.Get<0>())
			ClientItemLooted(LocalItemData);
	}
}

void UPlayerInventory::TakeAllItems()
{
}

void UPlayerInventory::DropItemOnTheGround(FItemData* ItemData, EItemDestination Initiator, AActor* OwningPlayer)
{
	switch (Initiator)
	{
	case EItemDestination::InventorySlot:
		RemoveItemFromInventoryArray(ItemData);
		break;
	}
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
                                                                EInputMethodType InputMethod, UInventoryCore* Sender,
                                                                AActor* OwningPlayer)
{
	//TODO проверь этот метод потом, ты его писал очень уставший может что сделал не так, не ленись!
	FItemData* LocalItemData = ItemData;
	if (InventoryUtility::IsItemClassValid(ItemData) && IsValid(Sender))
	{
		if (!AttemptUsingTransferredItem(LocalItemData, Sender))
		{
			if (Sender->CheckOwnerGold())
				if (!HasEnoughGold(ItemData))
					return MakeTuple(false, FText::FromString(MessageNotEnoughGold));


			auto PlayerEquipment = InventoryUtility::GetPlayerEquipment(OwningPlayer);
			if (!IsValid(PlayerEquipment) || (IsValid(PlayerEquipment) && !PlayerEquipment->
				TryToAddToPartialStack(ItemData)))
			{
				auto InventoryData = GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData));
				auto PartialStackData = InventoryUtility::HasPartialStack(InventoryData.Get<0>(), ItemData);
				if (PartialStackData.Get<0>())
					AddToStackInInventory(ItemData, PartialStackData.Get<1>());
				else
				{
					switch (InputMethod)
					{
					case EInputMethodType::RightClick:
						{
							auto EmptySlot = GetEmptyInventorySlot(ItemData);
							if (EmptySlot.Get<0>())
								AddItemToInventoryArray(ItemData, EmptySlot.Get<1>());
							else
								return MakeTuple(false, FText::FromString(MessageInventoryFull));
						}
						break;
					case EInputMethodType::DragAndDrop:
						AddItemToInventoryArray(ItemData, IsSlotData->Index);
						break;
					}
				}
			}

			AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));

			if (Sender->CheckOwnerGold() && CheckOwnerGold())
			{
				auto LocalValue = InventoryUtility::CalculateStackedItemValue(LocalItemData);
				RemoveGoldFromOwner(LocalValue);
				Sender->AddGoldToOwner(LocalValue);
			}

			Sender->RemoveItemFromInventoryArray(LocalItemData);
			if (IsValid(Sender))
				Sender->RemoveWeightFromInventory(InventoryUtility::CalculateStackedItemWeight(LocalItemData));

			return MakeTuple(true, FText::FromString(""));
		}
		return MakeTuple(true, FText::FromString(""));
	}
	return MakeTuple(false, FText::FromString(""));
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
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		switch (ItemData->Class.GetDefaultObject()->UseType)
		{
		case EItemUseType::Currency:
			{
				AddGoldToOwner(InventoryUtility::CalculateStackedItemValue(ItemData));
				Sender->RemoveItemFromInventoryArray(ItemData);
				if(OnItemUsed.IsBound())
					OnItemUsed.Broadcast(*ItemData);
			}
			return true;
		case EItemUseType::CraftingRecipe:
			{
				Sender->RemoveItemFromInventoryArray(ItemData);
				if(OnItemUsed.IsBound())
					OnItemUsed.Broadcast(*ItemData);
			}
			return true;
		default:
			return false;
		}
	}
	return false;
}

void UPlayerInventory::AddItemToInventoryArray(FItemData* ItemData, int32 Index)
{
	Super::AddItemToInventoryArray(ItemData, Index);
}

void UPlayerInventory::SwapItemsInInventory(FItemData* FirstItem, FItemData* SecondItem)
{
	Super::SwapItemsInInventory(FirstItem, SecondItem);
}
