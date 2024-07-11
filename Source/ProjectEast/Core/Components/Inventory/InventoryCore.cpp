#include "InventoryCore.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

//TODO Completed
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

//TODO Completed
void UInventoryCore::CallOnRefreshInventory(EInventoryPanels Panel) const
{
	if (OnRefreshInventory.IsBound())
		OnRefreshInventory.Broadcast(Panel);
}

void UInventoryCore::ClientTransferItemReturnValue(bool Success, FText FailureMessage)
{
}

void UInventoryCore::ClientUpdateItems(UInventoryCore* Inventory, EInventoryPanels InventoryPanel,
                                       TArray<FItemData*> Array)
{
}

void UInventoryCore::ClientUpdateAddedItem(FItemData* ItemData, UInventoryCore* Inventory)
{
}

void UInventoryCore::ClientUpdateRemovedItem(FItemData* ItemData, UInventoryCore* Inventory)
{
}

//TODO Completed
void UInventoryCore::SortInventory(ESortMethod Method, EInventoryPanels SinglePanel, bool EveryPanel)
{
	CurrentPanel = SinglePanel;
	if (EveryPanel)
	{
		//TODO Check for remove
		for (int i = 0; i < PanelsToUse.Num(); ++i)
			CurrentPanel = PanelsToUse[i];
	}

	CurrentInventory = GetInventoryAndSize(CurrentPanel).Get<0>();
	switch (Method)
	{
	case ESortMethod::Quicksort:
		CurrentInventory = InventoryUtility::QuickSortItems(CurrentInventory);
	case ESortMethod::ByType:
		CurrentInventory = InventoryUtility::SortItemsByType(CurrentInventory);
	case ESortMethod::ByRarity:
		CurrentInventory = InventoryUtility::SortItemsByRarity(CurrentInventory);
	case ESortMethod::ByValue:
		CurrentInventory = InventoryUtility::SortItemsByValue(CurrentInventory);
	case ESortMethod::ByWeight:
		CurrentInventory = InventoryUtility::SortItemsByWeight(CurrentInventory);
		break;
	}

	ApplyChangesToInventoryArray(CurrentPanel, CurrentSortedItems);
	BuildInventory(CurrentPanel);

	CallOnRefreshInventory(CurrentPanel);

	if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		UpdateViewInventory(CurrentPanel);
		ClientUpdateItems(this, CurrentPanel, GetInventoryAndSize(CurrentPanel).Get<0>());
	}
}

void UInventoryCore::UpdateViewInventory(EInventoryPanels InventoryPanel)
{
}

void UInventoryCore::ServerUpdateItems(AActor* Actor)
{
}

void UInventoryCore::MulticastSetCurrentWeight(float CurrentWeight)
{
}

void UInventoryCore::MulticastSetOwnerGold(float OwnerGold)
{
}

void UInventoryCore::AddViewer(APlayerState* PlayerState, UInventoryCore* Inventory)
{
}

void UInventoryCore::RemoveViewer(APlayerState* PlayerState, UInventoryCore* Inventory)
{
}

void UInventoryCore::ServerMoveItemToSlot(UInventoryCore* Inventory, EInventoryPanels InventoryPanels, int32 MoveFrom,
                                          int32 MoveTo)
{
}

void UInventoryCore::ServerAddItemToInventory(UInventoryCore* Inventory, FItemData* ItemData, int32 SlotIndex)
{
}

void UInventoryCore::ServerRemoveItemFromInventory(UInventoryCore* Inventory, FItemData ItemData)
{
}

void UInventoryCore::ServerTransferItemFromInventory(UInventoryCore* Receiver, FItemData* ItemData, FItemData* InSlotData, EInputMethodType Method, UInventoryCore* Sender, AActor* OwningPlayer)
{
}

void UInventoryCore::ServerTransferItemFromEquipment(FItemData* ItemData, FItemData* InSlotData, EInputMethodType Method,
                                                     UPlayerEquipment* Sender)
{
}

void UInventoryCore::ServerSplitItemsInInventory(UInventoryCore* Receiver, UInventoryCore* Sender, FItemData ItemData,
                                                 FItemData InSlotData, FItemData StackableLeft, EInputMethodType Method,
                                                 EInputMethodType Initiator,
                                                 EInputMethodType Destination, AActor* OwningPlayer)
{
}

void UInventoryCore::ServerConfirmationPopupAccepted(UInventoryCore* Receiver, UInventoryCore* Sender, FItemData ItemData,
                                                     FItemData InSlotData, EInputMethodType Method,
                                                     EInputMethodType Initiator, EInputMethodType Destination,
                                                     AActor* OwningPlayer)
{
}

void UInventoryCore::ServerSortInventory(UInventoryCore* Inventory, EInputMethodType Method,
                                         EInventoryPanels SinglePanel, bool EveryPanel)
{
}

//TODO Completed
void UInventoryCore::BeginPlay()
{
	Super::BeginPlay();
	InitializeInventory(nullptr);
}


//TODO Completed
void UInventoryCore::BuildInitialInventory()
{
	RandomizeInitialItems();

	for (int i = 0; i < AllItemsFromDT.Num(); ++i)
	{
		auto& DataTableItem = AllItemsFromDT[i];
		if (IsValid(&DataTableItem))
		{
			TArray<FName> Names = DataTableItem.GetRowNames();
			for (int a = 0; a < Names.Num(); ++a)
			{
				FItemData* ItemData = DataTableItem.FindRow<FItemData>(Names[a], TEXT(""));
				CurrentItemData = ItemData;

				auto DataEmptySlot = GetEmptyInventorySlot(CurrentItemData);
				if (DataEmptySlot.Get<0>())
				{
					AddItemToInventoryArray(CurrentItemData, DataEmptySlot.Get<1>());
					AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(CurrentItemData));
				}
			}
		}
	}

	for (int i = 0; i < SingleDTItem.Num(); ++i)
	{
		auto DataTable = SingleDTItem[i].TableAndRow.DataTable;
		auto RowName = SingleDTItem[i].TableAndRow.RowName;

		CurrentItemData = DataTable->FindRow<FItemData>(RowName,TEXT(""));
		CurrentItemData->Quantity = FMathf::Clamp(SingleDTItem[i].Quantity, 1, SingleDTItem[i].Quantity);
		
		auto DataEmptySlot = GetEmptyInventorySlot(CurrentItemData);
		if (DataEmptySlot.Get<0>())
		{
			AddItemToInventoryArray(CurrentItemData, DataEmptySlot.Get<1>());
			AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(CurrentItemData));
		}
	}

	AllItemsFromDT.Empty();
	SingleDTItem.Empty();
	RandomizedItemsData.Empty();

	if (bSortInitialItems)
	{
		SortInventory(DefaultSortingMethod, EInventoryPanels::P1, true);
	}
}

//TODO Completed
void UInventoryCore::BuildInventory(EInventoryPanels Panel)
{
	auto Data = GetInventoryAndSize(Panel);

	CurrentInventoryArray = Data.Get<0>();
	CurrentInventorySize = Data.Get<1>();

	if (bIsUseInventorySize)
	{
		CurrentInventoryArray.SetNum(CurrentInventorySize);

		for (int i = 0; i < CurrentInventoryArray.Num(); ++i)
		{
			auto& ItemData = CurrentInventoryArray[i];
			ItemData->Index = i;
			ItemData->bIsEquipped = false;
		}
		ApplyChangesToInventoryArray(Panel, CurrentInventoryArray);
	}
	else
	{
		for (int i = 0; i < CurrentInventoryArray.Num(); ++i)
		{
			auto& ItemData = CurrentInventoryArray[i];
			if (InventoryUtility::IsItemClassValid(ItemData))
			{
				auto IndexNewElement = CurrentEmptyInventoryArray.Add(ItemData);

				auto& NewItemData = CurrentEmptyInventoryArray[IndexNewElement];
				NewItemData->Index = IndexNewElement;
				NewItemData->bIsEquipped = false;
			}
		}
		ApplyChangesToInventoryArray(Panel, CurrentEmptyInventoryArray);
	}
}

//TODO Completed
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

//TODO Completed
TTuple<TArray<FItemData*>, int32> UInventoryCore::GetInventoryAndSize(EInventoryPanels Panel)
{
	if (PanelsToUse.Contains(Panel))
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
	}

	return MakeTuple(InventoryP1, InventorySizeP1);
}

void UInventoryCore::SwitchedActivePanel(EInventoryPanels Panel)
{
}

EItemRemoveType UInventoryCore::GetItemRemoveType(FItemData* ItemData) const
{
	if(bIsCheckRemoveType)
		return ItemData->Class.GetDefaultObject()->RemoveType;
	return EItemRemoveType::Default;
}

void UInventoryCore::RandomizeInitialItems()
{
}

//TODO Completed
TTuple<bool, int32> UInventoryCore::GetEmptyInventorySlot(const FItemData* ItemData)
{
	if (bIsUseInventorySize)
	{
		auto TupleElements = GetInventoryAndSize(InventoryUtility::GetInventoryPanelFromItem(ItemData));
		return InventoryUtility::FindEmptySlotInArray(TupleElements.Get<0>());
	}

	return MakeTuple(true, -1);
}

void UInventoryCore::AddItemToInventoryArray(FItemData* ItemData, int32 Index)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		ModifyItemValue(ItemData);

		auto InventoryPanel = InventoryUtility::GetInventoryPanelFromItem(ItemData);
		auto InventoryData = GetInventoryAndSize(InventoryPanel);

		CurrentInventoryArray = InventoryData.Get<0>();
		if (bIsUseInventorySize || Index >= 0)
		{
			ItemData->Index = Index;
			ItemData->bIsEquipped = false;
			CurrentInventoryArray[Index] = ItemData;

			ApplyChangesToInventoryArray(InventoryPanel, CurrentInventoryArray);


			if (UKismetSystemLibrary::IsStandalone(GetWorld()))
			{
				SwitchActivePanel(InventoryPanel);

				if (OnRefreshInventory.IsBound())
					OnRefreshInventory.Broadcast(InventoryPanel);
				if (OnHighlightInventorySlot.IsBound())
					OnHighlightInventorySlot.Broadcast(ItemData->Index);
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

					if (OnRefreshInventory.IsBound())
						OnRefreshInventory.Broadcast(InventoryPanel);
					if (OnHighlightInventorySlot.IsBound())
						OnHighlightInventorySlot.Broadcast(ItemData->Index);
				}
				else
				{
					UpdateViewItem(ItemData, false);
				}
			}
		}
	}
}

void UInventoryCore::AddWeightToInventory(float Weight)
{
}

void UInventoryCore::ModifyItemValue(FItemData* ItemData)
{
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
