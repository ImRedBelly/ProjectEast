﻿#include "InventoryCore.h"
#include "PlayerInventory.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Components/Equipment/PlayerEquipment.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UInventoryCore::InitializeInventory(APlayerController* PlayerController)
{
	//TODO Check use APlayerController

	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		for (auto Panel : PanelsToUse)
			BuildInventory(Panel);

		BuildInitialInventory();
	}
}

void UInventoryCore::CallOnRefreshInventory(EInventoryPanels Panel) const
{
	if (OnRefreshInventory.IsBound())
		OnRefreshInventory.Broadcast(Panel);
}

void UInventoryCore::CallOnAddedToInventorySlot(FItemData& ItemData) const
{
	if (OnAddedToInventoryArray.IsBound())
		OnAddedToInventoryArray.Broadcast(ItemData);
}

void UInventoryCore::CallOnRemovedFromInventoryArray(FItemData& ItemData) const
{
	if (OnRemovedFromInventoryArray.IsBound())
		OnRemovedFromInventoryArray.Broadcast(ItemData);
}

void UInventoryCore::CallOnHighlightSlot(int32 Index) const
{
	if (OnHighlightInventorySlot.IsBound())
		OnHighlightInventorySlot.Broadcast(Index);
}

EInventoryPanels UInventoryCore::GetActivePanel() const
{
	return ActivePanel;
}

void UInventoryCore::ClientTransferItemReturnValue(bool Success, FText FailureMessage)
{
	// TODO MOST BE EMPTY
}

void UInventoryCore::ClientUpdateItems(UInventoryCore* Inventory, EInventoryPanels InventoryPanel,
                                       TArray<FItemData*> Array) const
{
	if (IsValid(Inventory))
	{
		Inventory->ApplyChangesToInventoryArray(InventoryPanel, Array);
		CallOnRefreshInventory(InventoryPanel);
	}
}

void UInventoryCore::ClientUpdateAddedItem(FItemData* ItemData, UInventoryCore* Inventory)
{
	UpdateAddedItemInInventory(ItemData, Inventory);
}

void UInventoryCore::ClientUpdateRemovedItem(FItemData* ItemData, UInventoryCore* Inventory)
{
	UpdateRemovedItemInInventory(ItemData, Inventory);
}

void UInventoryCore::SortInventory(ESortMethod Method, EInventoryPanels SinglePanel, bool EveryPanel)
{
	if (EveryPanel)
	{
		for (int i = 0; i < PanelsToUse.Num(); ++i)
		{
			SinglePanel = PanelsToUse[i];
			SortInventoryByPanel(Method, SinglePanel);
		}
	}
	else
		SortInventoryByPanel(Method, SinglePanel);
}

void UInventoryCore::ServerUpdateItems(AActor* Actor)
{
	if (IsValid(Actor))
	{
		UInventoryCore* Inventory = Cast<UInventoryCore>(Actor->GetComponentByClass(StaticClass()));
		if (IsValid(Inventory))
		{
			for (int i = 0; i < Inventory->PanelsToUse.Num(); ++i)
			{
				ClientUpdateItems(Inventory, PanelsToUse[i], Inventory->GetInventoryAndSize(PanelsToUse[i]).Get<0>());
			}
		}
	}
}

void UInventoryCore::MulticastSetCurrentWeight(float CurrentWeight)
{
	CurrentInventoryWeight = CurrentWeight;
}

void UInventoryCore::MulticastSetOwnerGold(float Gold)
{
	this->OwnerGold = Gold;
}

void UInventoryCore::AddViewer(APlayerState* PlayerState, UInventoryCore* Inventory)
{
	if (IsValid(PlayerState))
		for (auto Viewer : Inventory->CurrentViewers)
			CurrentViewers.Add(Viewer);
}

void UInventoryCore::RemoveViewer(APlayerState* PlayerState, UInventoryCore* Inventory)
{
	if (IsValid(PlayerState))
		for (int i = 0; i < Inventory->CurrentViewers.Num(); ++i)
			CurrentViewers.Remove(Inventory->CurrentViewers[i]);
}

void UInventoryCore::ServerMoveItemToSlot(UInventoryCore* Inventory, EInventoryPanels Panel, int32 MoveFrom,
                                          int32 MoveTo)
{
	if (IsValid(Inventory))
	{
		FItemData* FromItem = Inventory->GetItemBySlot(Panel, MoveFrom);
		FItemData* ToItem = Inventory->GetItemBySlot(Panel, MoveTo);
		Inventory->SwapItemsInInventory(FromItem, ToItem);
	}
}

void UInventoryCore::ServerAddItemToInventory(UInventoryCore* Inventory, FItemData* ItemData, int32 SlotIndex)
{
	Inventory->AddItemToInventoryArray(ItemData, SlotIndex);
}

void UInventoryCore::ServerRemoveItemFromInventory(UInventoryCore* Inventory, FItemData* ItemData)
{
	Inventory->RemoveItemFromInventoryArray(ItemData);
}

void UInventoryCore::ServerTransferItemFromInventory(UInventoryCore* Receiver, FItemData* ItemData,
                                                     FItemData* InSlotData, EInputMethodType Method,
                                                     UInventoryCore* Sender, AActor* OwningPlayer)
{
	auto TransferData = Receiver->TransferItemFromInventory(ItemData, InSlotData, Method, Sender, OwningPlayer);
	ClientTransferItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>());
}

void UInventoryCore::ServerTransferItemFromEquipment(FItemData* ItemData, FItemData* InSlotData,
                                                     EInputMethodType Method, UPlayerEquipment* Sender)
{
	auto TransferData = TransferItemFromEquipment(ItemData, InSlotData, Method, Sender);
	ClientTransferItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>());
}

void UInventoryCore::ServerSplitItemsInInventory(UInventoryCore* Receiver, UInventoryCore* Sender, FItemData* ItemData,
                                                 FItemData* InSlotData, FItemData* StackableLeft,
                                                 EInputMethodType Method,
                                                 EItemDestination Initiator,
                                                 EItemDestination Destination, AActor* OwningPlayer)
{
	Receiver->SplitItemsInInventory(Sender, ItemData, InSlotData, StackableLeft, Method, Initiator, Destination,
	                                OwningPlayer);
}

void UInventoryCore::ServerConfirmationPopupAccepted(UInventoryCore* Receiver, UInventoryCore* Sender,
                                                     FItemData* ItemData,
                                                     FItemData* InSlotData, EInputMethodType Method,
                                                     EInputMethodType Initiator, EInputMethodType Destination,
                                                     AActor* OwningPlayer)
{
	Receiver->ConfirmationPopupAccepted(Sender, ItemData, InSlotData, Method, Initiator, Destination, OwningPlayer);
}

void UInventoryCore::UpdateViewersInventory(EInventoryPanels Panel)
{
	for (auto Viewer : CurrentViewers)
		if (auto PlayerInventory = InventoryUtility::GetPlayerInventory(Viewer->GetOwner()))
			PlayerInventory->ClientUpdateItems(this, Panel, GetInventoryAndSize(Panel).Get<0>());
}

void UInventoryCore::UpdateAddedItemInInventory(FItemData* ItemData, UInventoryCore* Inventory)
{
	FItemData* LocalItemData = ItemData;
	if (IsValid(Inventory))
	{
		auto LocalPanel = InventoryUtility::GetInventoryPanelFromItem(LocalItemData);
		auto LocalInventory = Inventory->GetInventoryAndSize(LocalPanel).Get<0>();
		if (Inventory->bIsUseInventorySize)
		{
			if (LocalInventory.IsValidIndex(LocalItemData->Index))
				LocalInventory[LocalItemData->Index] = LocalItemData;
		}
		else
		{
			LocalInventory.Add(LocalItemData);
		}

		Inventory->ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		Inventory->SwitchedActivePanel(InventoryUtility::GetInventoryPanelFromItem(LocalItemData));
		Inventory->CallOnRefreshInventory(LocalPanel);
		Inventory->CallOnAddedToInventorySlot(*LocalItemData);
		Inventory->CallOnHighlightSlot(LocalItemData->Index);
	}
}

void UInventoryCore::UpdateViewersItem(FItemData* ItemData, bool Remove)
{
	for (auto Viewer : CurrentViewers)
		if (auto PlayerInventory = InventoryUtility::GetPlayerInventory(Viewer->GetOwner()))
		{
			if (Remove)
				PlayerInventory->ClientUpdateRemovedItem(ItemData, this);
			else
				PlayerInventory->ClientUpdateAddedItem(ItemData, this);
		}
}

void UInventoryCore::UpdateRemovedItemInInventory(FItemData* ItemData, UInventoryCore* Inventory)
{
	FItemData* LocalItemData = ItemData;
	if (IsValid(Inventory))
	{
		auto LocalPanel = InventoryUtility::GetInventoryPanelFromItem(LocalItemData);
		auto LocalInventory = Inventory->GetInventoryAndSize(LocalPanel).Get<0>();
		if (Inventory->bIsUseInventorySize)
		{
			if (LocalInventory.IsValidIndex(LocalItemData->Index))
			{
				FItemData* NewItemData = new FItemData();
				NewItemData->Index = LocalItemData->Index;
				LocalInventory[LocalItemData->Index] = NewItemData;
			}
		}
		else
		{
			LocalInventory.Remove(LocalItemData);
		}

		Inventory->ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		Inventory->CallOnRemovedFromInventoryArray(*LocalItemData);
		Inventory->CallOnRefreshInventory(LocalPanel);
	}
}

void UInventoryCore::ServerSortInventory(UInventoryCore* Inventory, ESortMethod Method,
                                         EInventoryPanels SinglePanel, bool EveryPanel)
{
	Inventory->SortInventory(Method, SinglePanel, EveryPanel);
}

void UInventoryCore::BeginPlay()
{
	Super::BeginPlay();
	InitializeInventory(nullptr);
}

void UInventoryCore::BuildInitialInventory()
{
	// for (int i = 0; i < AllItemsFromDT.Num(); ++i)
	// {
	// 	auto& DataTableItem = AllItemsFromDT[i];
	// 	if (IsValid(DataTableItem))
	// 	{
	// 		TArray<FName> Names = DataTableItem->GetRowNames();
	// 		for (int a = 0; a < Names.Num(); ++a)
	// 		{
	// 			CurrentItemData = DataTableItem->FindRow<FItemData>(Names[a], TEXT(""));
	// 			FItemData* CurrentItemDataBase = InventoryUtility::CopyItemData(CurrentItemData);
	// 	
	// 			auto DataEmptySlot = GetEmptyInventorySlot(CurrentItemDataBase);
	// 			if (DataEmptySlot.Get<0>())
	// 			{
	// 				AddItemToInventoryArray(CurrentItemDataBase, DataEmptySlot.Get<1>());
	// 				AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(CurrentItemDataBase));
	// 			}
	// 		}
	// 	}
	// }

	RandomizeInitialItems();

	for (int i = 0; i < SingleDTItem.Num(); ++i)
	{
		auto DataTable = SingleDTItem[i].TableAndRow.DataTable;
		auto RowName = SingleDTItem[i].TableAndRow.RowName;

		FItemData* CurrentItemData = DataTable->FindRow<FItemData>(RowName,TEXT(""));

		FItemData* CurrentItemDataBase = InventoryUtility::CopyItemData(CurrentItemData);
		CurrentItemDataBase->Quantity = FMathf::Clamp(SingleDTItem[i].Quantity, 1, SingleDTItem[i].Quantity);

		auto DataEmptySlot = GetEmptyInventorySlot(CurrentItemDataBase);
		if (DataEmptySlot.Get<0>())
		{
			AddItemToInventoryArray(CurrentItemDataBase, DataEmptySlot.Get<1>());
			AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(CurrentItemDataBase));
		}
	}

	//AllItemsFromDT.Empty();
	SingleDTItem.Empty();
	RandomizedItemsData.Empty();

	if (bSortInitialItems)
	{
		SortInventory(DefaultSortingMethod, EInventoryPanels::P1, true);
	}
}

void UInventoryCore::SortInventoryByPanel(ESortMethod Method, EInventoryPanels Panel)
{
	TArray<FItemData*> LocaleSortedItems;
	TArray<FItemData*> CurrentInventory = GetInventoryAndSize(Panel).Get<0>();
	switch (Method)
	{
	case ESortMethod::Quicksort:
		LocaleSortedItems = InventoryUtility::QuickSortItems(CurrentInventory);
	case ESortMethod::ByType:
		LocaleSortedItems = InventoryUtility::SortItemsByType(CurrentInventory);
	case ESortMethod::ByRarity:
		LocaleSortedItems = InventoryUtility::SortItemsByRarity(CurrentInventory);
	case ESortMethod::ByValue:
		LocaleSortedItems = InventoryUtility::SortItemsByValue(CurrentInventory);
	case ESortMethod::ByWeight:
		LocaleSortedItems = InventoryUtility::SortItemsByWeight(CurrentInventory);
		break;
	}

	ApplyChangesToInventoryArray(Panel, LocaleSortedItems);
	BuildInventory(Panel);

	CallOnRefreshInventory(Panel);

	if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		UpdateViewersInventory(Panel);
		ClientUpdateItems(this, Panel, GetInventoryAndSize(Panel).Get<0>());
	}
}

void UInventoryCore::RemoveItemFromInventoryArray(FItemData* ItemData)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		auto NewInventory = InventoryUtility::GetInventoryPanelFromItem(ItemData);
		TArray<FItemData*> LocalInventory = GetInventoryAndSize(NewInventory).Get<0>();
		if (bIsUseInventorySize)
		{
			if (LocalInventory.IsValidIndex(ItemData->Index))
			{
				FItemData* NewItemData = new FItemData();
				NewItemData->Index = ItemData->Index;
				LocalInventory[ItemData->Index] = NewItemData;
			}
		}
		else
		{
			LocalInventory.Remove(ItemData);
		}


		ApplyChangesToInventoryArray(NewInventory, LocalInventory);

		if (UKismetSystemLibrary::IsStandalone(GetWorld()))
		{
			CallOnRemovedFromInventoryArray(*ItemData);
			CallOnRefreshInventory(NewInventory);
		}
		else
		{
			UpdateViewersItem(ItemData, true);
			ClientUpdateRemovedItem(ItemData, this);
		}
	}
}

void UInventoryCore::BuildInventory(EInventoryPanels Panel)
{
	auto Data = GetInventoryAndSize(Panel);

	TArray<FItemData*> LocalInventory = Data.Get<0>();
	int32 LocalSize = Data.Get<1>();

	if (bIsUseInventorySize)
	{
		for (int i = 0; i < LocalSize; ++i)
		{
			if (!LocalInventory.IsValidIndex(i))
				LocalInventory.Add(new FItemData());
			LocalInventory[i]->Index = i;
			LocalInventory[i]->bIsEquipped = false;
		}

		ApplyChangesToInventoryArray(Panel, LocalInventory);
	}
	else
	{
		TArray<FItemData*> CurrentEmptyInventoryArray;

		for (int i = 0; i < LocalInventory.Num(); ++i)
		{
			if (InventoryUtility::IsItemClassValid(LocalInventory[i]))
			{
				auto IndexNewElement = CurrentEmptyInventoryArray.Add(LocalInventory[i]);

				auto& NewItemData = CurrentEmptyInventoryArray[IndexNewElement];
				NewItemData->Index = IndexNewElement;
				NewItemData->bIsEquipped = false;
			}
		}
		ApplyChangesToInventoryArray(Panel, CurrentEmptyInventoryArray);
	}
}

void UInventoryCore::ApplyChangesToInventoryArray(EInventoryPanels Panel, TArray<FItemData*> Inventory)
{
	if (PanelsToUse.Contains(Panel))
	{
		switch (Panel)
		{
		case EInventoryPanels::P1:
			InventoryP1 = Inventory;
			break;
		case EInventoryPanels::P2:
			InventoryP2 = Inventory;
			break;
		case EInventoryPanels::P3:
			InventoryP3 = Inventory;
			break;
		case EInventoryPanels::P4:
			InventoryP4 = Inventory;
			break;
		default:
			InventoryP1 = Inventory;
			break;
		}
	}
	else
		InventoryP1 = Inventory;
}

TTuple<bool, FText> UInventoryCore::TransferItemFromInventory(FItemData* ItemData, FItemData* IsSlotData,
                                                              EInputMethodType InputMethod, UInventoryCore* Sender,
                                                              AActor* OwningPlayer)
{
	if (InventoryUtility::IsItemClassValid(ItemData) && IsValid(Sender))
	{
		if (Sender->CheckOwnerGold() && !HasEnoughGold(ItemData))
			return MakeTuple(false, FText::FromString(MessageNotEnoughGold));

		auto InventoryData = GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData));
		auto PartialData = InventoryUtility::HasPartialStack(InventoryData.Get<0>(), ItemData);
		if (PartialData.Get<0>())
			AddToStackInInventory(ItemData, PartialData.Get<1>());
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
					break;
				}
			case EInputMethodType::DragAndDrop:
				AddItemToInventoryArray(ItemData, IsSlotData->Index);
				break;
			}
		}

		AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));

		if (Sender->CheckOwnerGold() && CheckOwnerGold())
		{
			auto DeltaGold = InventoryUtility::CalculateStackedItemValue(ItemData);
			RemoveGoldFromOwner(DeltaGold);
			Sender->AddGoldToOwner(DeltaGold);
		}

		Sender->RemoveItemFromInventoryArray(ItemData);
		Sender->RemoveWeightFromInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));
		return MakeTuple(true, FText());
	}
	return MakeTuple(false, FText());
}

TTuple<bool, FText> UInventoryCore::TransferItemFromEquipment(FItemData* ItemData, FItemData* IsSlotData,
                                                              EInputMethodType InputMethod, UPlayerEquipment* Sender)
{
	if (InventoryUtility::IsItemClassValid(ItemData) && IsValid(Sender))
	{
		auto InventoryData = GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData));
		auto PartialData = InventoryUtility::HasPartialStack(InventoryData.Get<0>(), ItemData);
		if (PartialData.Get<0>())
			AddToStackInInventory(ItemData, PartialData.Get<1>());
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
					break;
				}
			case EInputMethodType::DragAndDrop:
				{
					AddItemToInventoryArray(ItemData, IsSlotData->Index);
				}
				break;
			}
		}

		Sender->RemoveItemFromEquipmentArray(ItemData);
		Sender->DetachItemFromEquipment(ItemData);

		return MakeTuple(true, FText());
	}

	return MakeTuple(false, FText());
}

void UInventoryCore::SwapItemsInInventory(FItemData* FirstItem, FItemData* SecondItem)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this) && InventoryUtility::IsItemClassValid(FirstItem))
	{
		if (InventoryUtility::IsItemClassValid(SecondItem))
		{
			if (InventoryUtility::AreItemsTheSame(FirstItem, SecondItem))
			{
				RemoveItemFromInventoryArray(FirstItem);
				AddToStackInInventory(FirstItem, SecondItem->Index);
			}
			else
				LocalSwapItemsInInventory(FirstItem, SecondItem);
		}
		else
			LocalSwapItemsInInventory(FirstItem, SecondItem);
	}
}

bool UInventoryCore::HasEnoughGold(FItemData* ItemData) const
{
	if (bIsCheckOwnerGold)
		return OwnerGold >= InventoryUtility::CalculateStackedItemValue(ItemData);
	return true;
}

void UInventoryCore::RemoveGoldFromOwner(float Gold)
{
	if (bIsCheckOwnerGold)
	{
		OwnerGold -= Gold;
		OwnerGold = FMathf::Clamp(OwnerGold, 0, OwnerGold);
		MulticastSetOwnerGold(OwnerGold);
		if (OnChangedOwnerGold.IsBound())
			OnChangedOwnerGold.Broadcast();
	}
}

void UInventoryCore::AddGoldToOwner(float Gold)
{
	if (bIsCheckOwnerGold)
	{
		OwnerGold += Gold;
		OwnerGold = FMathf::Clamp(OwnerGold, 0, OwnerGold);
		MulticastSetOwnerGold(OwnerGold);
		if (OnChangedOwnerGold.IsBound())
			OnChangedOwnerGold.Broadcast();
	}
}

TTuple<TArray<FItemData*>, int32> UInventoryCore::GetInventoryAndSize(EInventoryPanels Panel)
{
	if (PanelsToUse.Contains(Panel))
	{
		switch (Panel)
		{
		case EInventoryPanels::P1:
			return MakeTuple(InventoryP1, InventorySizeP1);
		case EInventoryPanels::P2:
			return MakeTuple(InventoryP2, InventorySizeP2);
		case EInventoryPanels::P3:
			return MakeTuple(InventoryP3, InventorySizeP3);
		case EInventoryPanels::P4:
			return MakeTuple(InventoryP4, InventorySizeP4);
		default:
			return MakeTuple(InventoryP1, InventorySizeP1);
		}
	}

	return MakeTuple(InventoryP1, InventorySizeP1);
}

void UInventoryCore::SwitchedActivePanel(EInventoryPanels Panel)
{
	if (Panel != ActivePanel && PanelsToUse.Contains(Panel))
	{
		ActivePanel = Panel;
		if (OnSwitchedActivePanel.IsBound())
			OnSwitchedActivePanel.Broadcast(ActivePanel);
	}
}

EItemRemoveType UInventoryCore::GetItemRemoveType(FItemData* ItemData) const
{
	if (bIsCheckRemoveType)
		return ItemData->Class.GetDefaultObject()->RemoveType;
	return EItemRemoveType::Default;
}

void UInventoryCore::RandomizeInitialItems()
{
	for (int i = 0; i < RandomizedItemsData.Num(); ++i)
	{
		TArray<FItemData*> DrawnItems;
		TArray<FItemData*> NotDrawnItems;

		if (IsValid(RandomizedItemsData[i].DataTable))
		{
			auto DataTable = RandomizedItemsData[i].DataTable;
			for (auto RowName : DataTable->GetRowNames())
			{
				FItemData* ItemData = DataTable->FindRow<FItemData>(RowName,TEXT(""));

				if (RandomizedItemsData[i].MaxLootItems > DrawnItems.Num())
				{
					if (InventoryUtility::IsItemClassValid(ItemData))
					{
						if (ItemData->Class.GetDefaultObject()->DropPercentage >=
							UKismetMathLibrary::RandomIntegerInRange(0, 100))
						{
							DrawnItems.Add(ItemData);
							RandomizeItemParameters(ItemData);
							auto SlotData = GetEmptyInventorySlot(ItemData);
							if (SlotData.Get<0>())
							{
								AddItemToInventoryArray(ItemData, SlotData.Get<1>());
								AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));
							}
						}
						else
						{
							NotDrawnItems.Add(ItemData);
						}
					}
				}
				else
					break;
			}

			if (DrawnItems.Num() < RandomizedItemsData[i].MinLootItems)
			{
				int32 Length = RandomizedItemsData[i].MinLootItems - DrawnItems.Num();
				for (int a = 0; a < Length; ++a)
				{
					if (NotDrawnItems.Num() >= 1)
					{
						auto NewItemData = NotDrawnItems[UKismetMathLibrary::RandomIntegerInRange(
							0, NotDrawnItems.Num() - 1)];
						FItemData* ItemData = InventoryUtility::CopyItemData(NewItemData);
						NotDrawnItems.Remove(NewItemData);

						RandomizeItemParameters(ItemData);
						auto SlotData = GetEmptyInventorySlot(ItemData);
						if (SlotData.Get<0>())
						{
							AddItemToInventoryArray(ItemData, SlotData.Get<1>());
							AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));
						}
					}
				}
			}
		}
	}
}

FItemData* UInventoryCore::RandomizeItemParameters(FItemData* ItemData)
{
	TArray<TSubclassOf<UMainItemData>> LootClasses = ItemData->Class.GetDefaultObject()->LootClasses;
	if (LootClasses.Num() != 0)
	{
		auto LootClass = LootClasses[UKismetMathLibrary::RandomIntegerInRange(0, LootClasses.Num() - 1)];
		ItemData->Class = LootClass;
	}

	float ValueQuantity = UKismetMathLibrary::RandomIntegerInRange(ItemData->Class.GetDefaultObject()->MinRandQuantity,
	                                                               ItemData->Class.GetDefaultObject()->MaxRandQuantity);
	ValueQuantity = FMathf::Clamp(ValueQuantity, 1, ValueQuantity);
	ItemData->Quantity = ValueQuantity;

	float ValueDurability = UKismetMathLibrary::RandomIntegerInRange(
		ItemData->Class.GetDefaultObject()->MinRandDurability,
		ItemData->Class.GetDefaultObject()->MaxRandDurability);

	ValueDurability = FMathf::Clamp(ValueDurability, 1, 100);
	ItemData->Durability = ValueDurability;

	return ItemData;
}

void UInventoryCore::SplitItemsInInventory(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
                                           FItemData* StackableLeft, EInputMethodType Method,
                                           EItemDestination Initiator, EItemDestination Destination,
                                           AActor* OwningPlayer)
{
	auto TransferData = TransferItemFromInventory(ItemData, InSlotData, Method, Sender, OwningPlayer);
	Sender->ClientTransferItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>());
	if (TransferData.Get<0>())
		if (InventoryUtility::IsStackableAndHaveStacks(StackableLeft, 0))
			Sender->AddItemToInventoryArray(StackableLeft, StackableLeft->Index);
}

void UInventoryCore::ConfirmationPopupAccepted(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
                                               EInputMethodType Method, EInputMethodType Initiator,
                                               EInputMethodType Destination, AActor* OwningPlayer)
{
	if (InventoryUtility::IsStackableAndHaveStacks(ItemData, 1))
	{
		auto TransferData = TransferItemFromInventory(ItemData, InSlotData, Method, Sender, OwningPlayer);
		Sender->ClientTransferItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>());
	}
}

TTuple<bool, int32> UInventoryCore::GetEmptyInventorySlot(const FItemData* ItemData)
{
	if (bIsUseInventorySize)
	{
		auto TupleElements = GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData));
		return InventoryUtility::FindEmptySlotInArray(TupleElements.Get<0>());
	}

	return MakeTuple(true, -1);
}

void UInventoryCore::ChangeInventorySize(EInventoryPanels Panels, int32 Size)
{
}

void UInventoryCore::OnRep_OwnerGold()
{
}

void UInventoryCore::OnRep_InventorySizeP1()
{
}

void UInventoryCore::OnRep_InventorySizeP2()
{
}

void UInventoryCore::OnRep_InventorySizeP3()
{
}

void UInventoryCore::OnRep_InventorySizeP4()
{
}

void UInventoryCore::OnRep_CurrentInventoryWeight()
{
}

void UInventoryCore::OnRep_MaxInventoryWeight()
{
}

void UInventoryCore::AddItemToInventoryArray(FItemData* ItemData, int32 SlotIndex)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		ModifyItemValue(ItemData);

		auto InventoryPanel = InventoryUtility::GetInventoryPanelFromItem(ItemData);
		auto InventoryData = GetInventoryAndSize(InventoryPanel);

		TArray<FItemData*> CurrentInventoryArray = InventoryData.Get<0>();
		if (bIsUseInventorySize || SlotIndex >= 0)
		{
			FItemData* NewItemData = InventoryUtility::CopyItemData(ItemData);
			NewItemData->Index = SlotIndex;
			NewItemData->bIsEquipped = false;
			CurrentInventoryArray[SlotIndex] = NewItemData;

			ApplyChangesToInventoryArray(InventoryPanel, CurrentInventoryArray);


			if (UKismetSystemLibrary::IsStandalone(GetWorld()))
			{
				SwitchActivePanel(InventoryPanel);
				CallOnRefreshInventory(InventoryPanel);
				CallOnHighlightSlot(NewItemData->Index);
			}
			else
			{
				UpdateViewItem(ItemData, false);
			}
		}
		else
		{
			if (InventoryUtility::IsItemClassValid(ItemData))
			{
				auto CurrentIndex = CurrentInventoryArray.Add(ItemData);
				auto NewItemData = CurrentInventoryArray[CurrentIndex];
				NewItemData->Index = CurrentIndex;
				NewItemData->bIsEquipped = false;
				auto NewInventoryPanel = InventoryUtility::GetInventoryPanelFromItem(NewItemData);
				ApplyChangesToInventoryArray(NewInventoryPanel, CurrentInventoryArray);

				if (UKismetSystemLibrary::IsStandalone(GetWorld()))
				{
					SwitchActivePanel(InventoryPanel);
					CallOnRefreshInventory(InventoryPanel);
					CallOnAddedToInventorySlot(*ItemData);
					CallOnHighlightSlot(ItemData->Index);
				}
				else
				{
					UpdateViewItem(ItemData, false);
				}
			}
		}
	}
}

void UInventoryCore::AddToStackInInventory(FItemData* ItemData, int32 Index)
{
	auto NewItemData = InventoryUtility::CopyItemData(ItemData);
	NewItemData->Index = Index;
	NewItemData->Quantity = GetItemBySlot(InventoryUtility::GetInventoryPanelFromItem(ItemData), Index)->Quantity +
		ItemData->Quantity;
	AddItemToInventoryArray(NewItemData, Index);
}

void UInventoryCore::AddWeightToInventory(float Weight)
{
	if (bIsUseInventoryWeight)
	{
		CurrentInventoryWeight += Weight;
		CurrentInventoryWeight = FMathf::Clamp(CurrentInventoryWeight, 0, CurrentInventoryWeight);
		MulticastSetCurrentWeight(CurrentInventoryWeight);
		if (OnChangedCurrentWeight.IsBound())
			OnChangedCurrentWeight.Broadcast();
	}
}

void UInventoryCore::RemoveWeightFromInventory(float Weight)
{
	if (bIsUseInventoryWeight)
	{
		CurrentInventoryWeight -= Weight;
		CurrentInventoryWeight = FMathf::Clamp(CurrentInventoryWeight, 0, CurrentInventoryWeight);
		MulticastSetCurrentWeight(CurrentInventoryWeight);
		if (OnChangedCurrentWeight.IsBound())
			OnChangedCurrentWeight.Broadcast();
	}
}

bool UInventoryCore::IsInventoryOverweight() const
{
	if (bIsUseInventoryWeight)
		return CurrentInventoryWeight > MaxInventoryWeight;
	return false;
}

TTuple<bool, int32> UInventoryCore::GetEmptyInventorySlot(FItemData* ItemData)
{
	if (bIsUseInventorySize)
	{
		auto Inventory = GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData)).Get<0>();
		auto EmptySlot = InventoryUtility::FindEmptySlotInArray(Inventory);
		if (EmptySlot.Get<0>())
			return MakeTuple(true, EmptySlot.Get<1>());

		return MakeTuple(false, 0);
	}
	return MakeTuple(true, -1);
}

FItemData* UInventoryCore::GetItemBySlot(EInventoryPanels Panel, int32 SlitIndex)
{
	auto InventoryData = GetInventoryAndSize(Panel).Get<0>();
	if (InventoryData.IsValidIndex(SlitIndex))
		return InventoryData[SlitIndex];
	return InventoryData[0];
}

TTuple<FItemData*, bool> UInventoryCore::GetItemByID(FString ID, EInventoryPanels InSpecifiedPanel)
{
	if (InSpecifiedPanel != EInventoryPanels::None)
	{
		auto DataByID = InventoryUtility::FindItemByID(GetInventoryAndSize(InSpecifiedPanel).Get<0>(), ID);
		if (DataByID.Get<0>())
			return MakeTuple(DataByID.Get<1>(), true);
		return MakeTuple(nullptr, false);
	}

	for (int i = 0; i < PanelsToUse.Num(); ++i)
	{
		auto DataByID = InventoryUtility::FindItemByID(GetInventoryAndSize(InSpecifiedPanel).Get<0>(), ID);
		if (DataByID.Get<0>())
			return MakeTuple(DataByID.Get<1>(), true);
	}

	return MakeTuple(nullptr, false);
}

TArray<FItemData*> UInventoryCore::GetCombinedInventories() const
{
	TArray<FItemData*> LocalInventory;
	LocalInventory.Append(InventoryP1);
	LocalInventory.Append(InventoryP2);
	LocalInventory.Append(InventoryP3);
	LocalInventory.Append(InventoryP4);
	return LocalInventory;
}

TTuple<FItemData*, bool> UInventoryCore::GetItemByData(FItemData* ItemData)
{
	auto InventoryData = GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData));
	auto FindItemIndex = InventoryUtility::FindItemIndex(InventoryData.Get<0>(), ItemData);
	if (FindItemIndex.Get<0>())
		return MakeTuple(InventoryData.Get<0>()[FindItemIndex.Get<1>()], true);

	return MakeTuple(nullptr, false);
}

TArray<FItemData*> UInventoryCore::GetItemsOfSpecifiedType(EItemsType ItemsType) const
{
	return InventoryUtility::GetAllItemsOfType(GetCombinedInventories(), ItemsType);
}

int32 UInventoryCore::GetAmountOfEmptySlots(EInventoryPanels Panels)
{
	return InventoryUtility::FindAmountOfEmptySlots(GetInventoryAndSize(Panels).Get<0>());
}


void UInventoryCore::ModifyItemValue(FItemData* ItemData) const
{
	ItemData->ValueModifier = ValueModifier;
}

void UInventoryCore::SwitchActivePanel(EInventoryPanels Panel)
{
	if (ActivePanel != Panel && PanelsToUse.Contains(Panel))
	{
		ActivePanel = Panel;
		if (OnSwitchedActivePanel.IsBound())
			OnSwitchedActivePanel.Broadcast(ActivePanel);
	}
}

void UInventoryCore::RemoveItemQuantity(FItemData* ItemData, int32 Quantity)
{
	FItemData* LocalItemData = ItemData;


	if (UKismetMathLibrary::Greater_IntInt(LocalItemData->Quantity, Quantity))
	{
		RemoveWeightFromInventory(InventoryUtility::CalculateStackedItemWeight(LocalItemData));
		ItemData->Quantity -= Quantity;
		AddItemToInventoryArray(ItemData, ItemData->Index);
		AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(LocalItemData));
	}

	else if (UKismetMathLibrary::LessEqual_IntInt(LocalItemData->Quantity, Quantity))
	{
		RemoveItemFromInventoryArray(ItemData);
		RemoveWeightFromInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));
	}
}

void UInventoryCore::UpdateViewItem(FItemData* ItemData, bool IsRemove)
{
	for (auto PlayerState : CurrentViewers)
	{
		auto PlayerInventory = InventoryUtility::GetPlayerInventory(PlayerState->GetOwner());
		if (IsValid(PlayerInventory))
		{
			if (IsRemove)
				ClientUpdateRemovedItem(ItemData, this);
			else
				ClientUpdateAddedItem(ItemData, this);
		}
	}
}

void UInventoryCore::LocalSwapItemsInInventory(FItemData* LocalFirstItem, FItemData* LocalSecondItem)
{
	EInventoryPanels InventoryPanel = InventoryUtility::GetInventoryPanelFromItem(LocalFirstItem);
	TArray<FItemData*> LocalInventoryData = GetInventoryAndSize(InventoryPanel).Get<0>();

	FItemData* NewFirstItemData = InventoryUtility::CopyItemData(LocalFirstItem);
	NewFirstItemData->Index = LocalSecondItem->Index;
	NewFirstItemData->bIsEquipped = false;
	if (LocalInventoryData.IsValidIndex(LocalSecondItem->Index))
		LocalInventoryData[LocalSecondItem->Index] = NewFirstItemData;

	FItemData* NewSecondItemData = InventoryUtility::CopyItemData(LocalSecondItem);
	NewSecondItemData->Index = LocalFirstItem->Index;
	NewSecondItemData->bIsEquipped = false;
	if (LocalInventoryData.IsValidIndex(LocalFirstItem->Index))
		LocalInventoryData[LocalFirstItem->Index] = NewSecondItemData;

	ApplyChangesToInventoryArray(InventoryPanel, LocalInventoryData);
	CallOnRefreshInventory(InventoryPanel);

	if (!UKismetSystemLibrary::IsStandalone(this))
		UpdateViewersInventory(InventoryPanel);
}
