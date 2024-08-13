#include "PlayerInventory.h"
#include "PlayerEquipment.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Actors/Environment/Miscellaneous/LootBag.h"
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
	SpawnLootBagNearThePlayer(ItemData, OwningPlayer);
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
	MainDroppedIndex = DroppedIndex;
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
		CashedPlayerController->GetWidgetManager()->DisplayMessageNotify(FailureMessage.ToString());
}

void UPlayerInventory::CloseInventoryWidget()
{
	
}

void UPlayerInventory::InputCloseWidget()
{
	if (bIsInteractableActorWidgetOpen)
		CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());
	else
		CashedPlayerController->GetWidgetManager()->CloseActiveWidget();
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

	if (CashedPlayerController->GetWidgetManager()->GetActiveWidget() == EWidgetType::Inventory)
		CloseInventoryWidget();

	else
	{
		if (bIsInteractableActorWidgetOpen)
			CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());

		CashedPlayerController->GetWidgetManager()->SwitchWidgetTo(EWidgetType::Inventory);
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

	if (IsValid(CashedPlayerController->GetWidgetManager()->GetMainWindow()))
	{
		UMainWindow* MainWindow = CashedPlayerController->GetWidgetManager()->GetMainWindow();
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
	if (IsValid(CashedPlayerController->GetWidgetManager()->GetMainWindow()))
	{
		auto MainWindow = CashedPlayerController->GetWidgetManager()->GetMainWindow();
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
	bIsInteractableActorWidgetOpen = true;
}

void UPlayerInventory::CloseStorageWidget()
{
	bIsInteractableActorWidgetOpen = false;
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
	case EItemDestination::EquipmentSlot:
		if(UPlayerEquipment* PlayerEquipment = InventoryUtility::GetPlayerEquipment(OwningPlayer))
		{
			PlayerEquipment->RemoveItemFromEquipmentArray(ItemData);
			PlayerEquipment->DetachItemFromEquipment(ItemData);
		}
		break;
	default: ;
	}
	
	RemoveWeightFromInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));
	ServerSpawnLootBag(ItemData, OwningPlayer);
}

void UPlayerInventory::SpawnLootBagNearThePlayer(FItemData* ItemData, AActor* OwningPlayer)
{
	if(AMainPlayerController* PlayerController = Cast<AMainPlayerController>(OwningPlayer))
	{
		if(MainDroppedIndex == 0)
		{
			auto Pawn = PlayerController->GetPawn();
			FVector StartPosition = (Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 50) - FVector(0,0,50);
			FVector EndPosition = StartPosition - FVector(0,0,100);

			auto LootBag = IsCollidingWithLootBag(StartPosition, EndPosition);
			if(IsValid(LootBag))
				CachedLootBag = LootBag;
			else
			{
				FActorSpawnParameters Parameters;
				Parameters.Owner = OwningPlayer;
				Parameters.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
				Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
				CachedLootBag = GetWorld()->SpawnActor<ALootBag>(DefaultLootBag, StartPosition, FRotator::ZeroRotator, Parameters);				
			}
		}
		
		MainDroppedIndex++;
		if(UInventoryCore* Inventory = Cast<UInventoryCore>(CachedLootBag->GetComponentByClass(UInventoryCore::StaticClass())))
		{
			auto InventoryData = Inventory->GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData));
			auto PartialData = InventoryUtility::HasPartialStack(InventoryData.Get<0>(), ItemData);
			if(PartialData.Get<0>())
				Inventory->AddItemToInventoryArray(ItemData, PartialData.Get<1>());
			else
				Inventory->AddItemToInventoryArray(ItemData, -1);
		}
	}
}

void UPlayerInventory::SpawnItemMeshNearThePlayer()
{
}

ALootBag* UPlayerInventory::IsCollidingWithLootBag(FVector StartPosition, FVector EndPosition) const
{
	FHitResult Result;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartPosition, EndPosition, 
											UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
											TArray<AActor*>(), EDrawDebugTrace::None, Result, true);
	if(Result.bBlockingHit)
		if(auto LootBag = Cast<ALootBag>(Result.GetActor()))
			return LootBag;
	
	return nullptr;
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
                                             EItemDestination Initiator, EItemDestination Destination,
                                             AActor* OwningPlayer)
{
	switch (Initiator)
	{
	case EItemDestination::InventorySlot:
		{
			switch (Destination)
			{
			case EItemDestination::InventorySlot:
				{
					RemoveItemFromInventoryArray(ItemData);
					if (InventoryUtility::IsItemClassValid(InSlotData))
						AddToStackInInventory(ItemData, InSlotData->Index);
					else
						AddItemToInventoryArray(ItemData, InSlotData->Index);
				}
				break;
			case EItemDestination::DropBar:
				DropItemOnTheGround(ItemData, EItemDestination::InventorySlot, OwningPlayer);
				break;
			}
			
			if (InventoryUtility::IsStackableAndHaveStacks(StackableLeft, 0))
				AddItemToInventoryArray(StackableLeft, StackableLeft->Index);
		}
		break;
	case EItemDestination::EquipmentSlot:
		{
			auto PlayerEquipment = InventoryUtility::GetPlayerEquipment(OwningPlayer);
			if(IsValid(PlayerEquipment))
			{
				switch (Destination)
				{
				case EItemDestination::InventorySlot:
					{
						PlayerEquipment->RemoveItemFromEquipmentArray(ItemData);
						if(InventoryUtility::IsItemClassValid(ItemData))
							AddToStackInInventory(ItemData, InSlotData->Index);
						else
							AddItemToInventoryArray(ItemData, InSlotData->Index);

						if (InventoryUtility::IsStackableAndHaveStacks(StackableLeft, 0))
							PlayerEquipment->AddItemToEquipmentArray(StackableLeft, StackableLeft->EquipmentSlot);
					}
					break;
				case EItemDestination::DropBar:
					{
						DropItemOnTheGround(ItemData, EItemDestination::EquipmentSlot, OwningPlayer);
						if (InventoryUtility::IsStackableAndHaveStacks(StackableLeft, 0))
							PlayerEquipment->AddItemToEquipmentArray(StackableLeft, StackableLeft->EquipmentSlot);
					}
					break;
				}
			}
		}
		break;
	case EItemDestination::VendorSlot:
	case EItemDestination::StorageSlot:
		auto Data = TransferItemFromInventory(ItemData, InSlotData, Method, Sender, OwningPlayer);
		ClientTransferItemReturnValue(Data.Get<0>(), Data.Get<1>());
		if(Data.Get<0>())
			if(InventoryUtility::IsStackableAndHaveStacks(StackableLeft, 0))
				Sender->AddItemToInventoryArray(StackableLeft, StackableLeft->Index);
		break;
	}
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
