#include "CraftingCore.h"

#include "PlayerCrafting.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingCore::BeginPlay()
{
	Super::BeginPlay();
	CreateCraftingList();
}

void UCraftingCore::ItemCrafted(AActor* OwningPlayer)
{
	FCraftingData* FirstItemInQueue = GetFirstItemFromQueue();
	FCraftingData* AAA = FirstItemInQueue;
	if (OnItemCrafted.IsBound())
		OnItemCrafted.Broadcast(*FirstItemInQueue, OwningPlayer);

	GetWorld()->GetTimerManager().ClearTimer(CraftingTimer);
	DecreaseCraftingCounter(FirstItemInQueue);
	
	if (!TryToStartCraftingProcess())
		FinishCraftingProcess();

	if (OnRefreshed.IsBound())
		OnRefreshed.Broadcast();
}

void UCraftingCore::TryToCraftCurrentItem()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		UPlayerCrafting* PlayerCrafting = InventoryUtility::GetPlayerCrafting(PlayerController);

		if (IsValid(PlayerCrafting))
		{
			if (PlayerCrafting->CanCraft(this))
			{
				PlayerCrafting->ServerTryToCraftItem(this, GetFirstItemFromQueue(), PlayerController);
			}
			else
			{
				StopCraftingProcess(GetFirstItemFromQueue(), PlayerController);
			}
		}
	}
}

void UCraftingCore::SpawnCraftedItem(FCraftingData* CraftingData, AActor* OwningPlayer) const
{
	if (OnSpawnCraftedItem.IsBound())
	{
		FCraftingData* Data = InventoryUtility::CopyCraftingData(CraftingData);
		OnSpawnCraftedItem.Broadcast(*Data, OwningPlayer);
	}
}

void UCraftingCore::FailedToInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft) const
{
	if (OnFailedToInitialize.IsBound())
	{
		FCraftingData* Data = InventoryUtility::CopyCraftingData(CraftingData);
		OnFailedToInitialize.Broadcast(*Data, AmountToCraft);
	}
}

void UCraftingCore::InitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft)
{
	if (AmountToCraft > 0)
	{
		CraftingData->CraftingCounter = AmountToCraft;
		CraftingData->MaxCount = AmountToCraft;
		AddToCraftingQueue(CraftingData);
	}
}

TArray<FCraftingData*> UCraftingCore::FilterByRarity(TArray<FCraftingData*> CraftingData)
{
	TArray<FCraftingData*> Common;
	TArray<FCraftingData*> Superior;
	TArray<FCraftingData*> Epic;
	TArray<FCraftingData*> Legendary;

	for (auto Data : CraftingData)
	{
		auto CraftableData = InventoryUtility::GetCraftableData(Data);
		if (CraftableData.Get<0>().Num() > 0)
		{
			FItemData* ItemData = CraftableData.Get<0>()[0];
			if (InventoryUtility::IsItemClassValid(ItemData))
			{
				switch (ItemData->Class.GetDefaultObject()->Rarity)
				{
				case EItemRarity::Common:
					Common.Add(Data);
					break;
				case EItemRarity::Superior:
					Superior.Add(Data);
					break;
				case EItemRarity::Epic:
					Epic.Add(Data);
					break;
				case EItemRarity::Legendary:
					Legendary.Add(Data);
					break;
				}
			}
		}
	}
	TArray<FCraftingData*> AllCraftingItems;
	AllCraftingItems.Append(Common);
	AllCraftingItems.Append(Superior);
	AllCraftingItems.Append(Epic);
	AllCraftingItems.Append(Legendary);

	return AllCraftingItems;
}

void UCraftingCore::CreateCraftingList()
{
	CraftingList.Empty();

	for (UDataTable* CraftingDataTable : CraftingDataTables)
	{
		for (auto RowName : CraftingDataTable->GetRowNames())
		{
			FCraftingData* CraftingData = CraftingDataTable->FindRow<FCraftingData>(RowName, "");
			if (InventoryUtility::IsCraftingDataValid(CraftingData))
				CraftingList.Add(CraftingData);
		}
	}
	for (auto TableRow : SingleDTItems)
	{
		FCraftingData* CraftingData = InventoryUtility::GetCraftingDataFromTableRow(TableRow);
		if (InventoryUtility::IsCraftingDataValid(CraftingData))
			CraftingList.Add(CraftingData);
	}

	CraftingList = FilterByRarity(CraftingList);
}

bool UCraftingCore::IsCrafting() const
{
	return bIsCrafting;
}

bool UCraftingCore::TryToStartCraftingProcess()
{
	FCraftingData* FirstItem = GetFirstItemFromQueue();
	if (InventoryUtility::IsCraftingDataValid(FirstItem))
	{
		return StartCraftingProcess(FirstItem);
	}
	return false;
}

bool UCraftingCore::CanStartCraftingProcess(FCraftingData* CraftingData) const
{
	return !GetWorld()->GetTimerManager().IsTimerActive(CraftingTimer) &&
		CraftingData->CraftingCounter > 0;
}

bool UCraftingCore::StartCraftingProcess(FCraftingData* CraftingData)
{
	if (CanStartCraftingProcess(CraftingData))
	{
		StartCraftingTimer(CraftingData->CraftingTime);
		bIsCrafting = true;
		if (OnCraftingProcessStarted.IsBound())
		{
			OnCraftingProcessStarted.Broadcast(*CraftingData);
		}

		return true;
	}
	return false;
}

void UCraftingCore::StopCraftingProcess(FCraftingData* CraftingData, AActor* OwningPlayer)
{
	if (IsCurrentlyCrafted(CraftingData))
	{
		GetWorld()->GetTimerManager().ClearTimer(CraftingTimer);
		bIsCrafting = false;
	}
	UPlayerCrafting* PlayerCrafting = InventoryUtility::GetPlayerCrafting(OwningPlayer);
	if (IsValid(PlayerCrafting))
		PlayerCrafting->ServerReturnSecuredMaterials(CraftingData, this, OwningPlayer);

	if (OnCraftingProcessStopped.IsBound())
	{
		FCraftingData* Data = InventoryUtility::CopyCraftingData(CraftingData);
		OnCraftingProcessStopped.Broadcast(*Data);
	}
	RemoveFromCraftingQueue(CraftingData);
}

void UCraftingCore::FinishCraftingProcess()
{
	bIsCrafting = false;
	FCraftingData* FirstItem = GetFirstItemFromQueue();
	if (OnCraftingProcessFinished.IsBound())
	{
		FCraftingData* Data = InventoryUtility::CopyCraftingData(FirstItem);
		OnCraftingProcessFinished.Broadcast(*Data);
	}
	RemoveFromCraftingQueue(FirstItem);
}

TTuple<float, float> UCraftingCore::GetCraftingProcessTime() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(CraftingTimer))
	{
		auto CurrentTime = GetWorld()->GetTimerManager().GetTimerElapsed(CraftingTimer);
		auto LeftTime = GetWorld()->GetTimerManager().GetTimerRemaining(CraftingTimer);
		return MakeTuple(CurrentTime, LeftTime);
	}

	return MakeTuple(0, 0);
}

void UCraftingCore::AddToCraftingQueue(FCraftingData* AddData)
{
	if (!IsCraftableItemInQueue(AddData))
	{
		auto Quantity = CraftingQueue.AddUnique(AddData);
		if (OnAddedToQueue.IsBound())
		{
			OnAddedToQueue.Broadcast(*AddData, Quantity);
		}
	}
	else
	{
		auto ModifyData = ModifyCraftingCounter(AddData->CraftingID, AddData->CraftingCounter, AddData->MaxCount);
		if (OnAddedToQueue.IsBound())
		{
			OnAddedToQueue.Broadcast(*ModifyData, GetItemQueueIndex(AddData->CraftingID));
		}
	}
	TryToStartCraftingProcess();
}

void UCraftingCore::RemoveFromCraftingQueue(FCraftingData* RemoveData)
{
	int32 Index = GetItemQueueIndex(RemoveData->CraftingID);
	if (Index > -1)
	{
		CraftingQueue.RemoveAt(Index);
		if (OnRemoveFromQueue.IsBound())
		{
			FCraftingData* Data = InventoryUtility::CopyCraftingData(RemoveData);
			OnRemoveFromQueue.Broadcast(*Data, Index);
		}
		TryToStartCraftingProcess();
	}
}

void UCraftingCore::ClearCraftingQueue(AActor* OwningPlayer)
{
	UPlayerCrafting* PlayerCrafting = InventoryUtility::GetPlayerCrafting(OwningPlayer);
	if (IsValid(PlayerCrafting))
	{
		GetWorld()->GetTimerManager().ClearTimer(CraftingTimer);
		bIsCrafting = false;

		for (auto CraftingData : CraftingQueue)
			PlayerCrafting->ServerReturnSecuredMaterials(CraftingData, this, OwningPlayer);
		CraftingQueue.Empty();
		if (OnClearedQueue.IsBound())
			OnClearedQueue.Broadcast();
		if (OnRefreshed.IsBound())
			OnRefreshed.Broadcast();
	}
}

TTuple<bool, FCraftingData*> UCraftingCore::GetItemFromQueueByID(FString ID)
{
	int32 Index = GetItemQueueIndex(ID);
	if (Index > -1)
	{
		FCraftingData* Data = InventoryUtility::CopyCraftingData(CraftingQueue[Index]);
		return MakeTuple(true, Data);
	}

	return MakeTuple(false, &EmptyCraftingData);
}

FCraftingData* UCraftingCore::GetFirstItemFromQueue()
{
	if (CraftingQueue.IsValidIndex(0))
		return CraftingQueue[0];
	return &EmptyCraftingData;
}

FCraftingData* UCraftingCore::ModifyCraftingCounter(FString CraftingID, int32 CraftingCounter, int32 MaxCount)
{
	int32 Index = GetItemQueueIndex(CraftingID);
	if (Index > -1)
	{
		FCraftingData* Data = InventoryUtility::CopyCraftingData(CraftingQueue[Index]);
		Data->CraftingCounter += CraftingCounter;
		Data->MaxCount += MaxCount;
		return Data;
	}

	return &EmptyCraftingData;
}

TArray<FCraftingData*> UCraftingCore::GetCraftingListArray()
{
	return CraftingList;
}

void UCraftingCore::StartCraftingTimer(float CraftingTime)
{
	float TimeValue = CraftingTime * (1 / CraftingDurationRate);
	float Time = FMathf::Clamp(TimeValue, 1, TimeValue);
	GetWorld()->GetTimerManager().SetTimer(CraftingTimer, this, &UCraftingCore::TryToCraftCurrentItem, Time, false);
}

bool UCraftingCore::IsCurrentlyCrafted(FCraftingData* CraftingData)
{
	return GetFirstItemFromQueue()->CraftingID == CraftingData->CraftingID;
}

void UCraftingCore::DecreaseCraftingCounter(FCraftingData* CraftingData)
{
	if (InventoryUtility::IsCraftingDataValid(CraftingData))
	{
		auto Index = GetItemQueueIndex(CraftingData->CraftingID);
		if (Index > -1)
		{
			CraftingQueue[Index]->CraftingCounter -= 1;
		}
	}
}

bool UCraftingCore::HasPlayerEnoughGold(FCraftingData* CraftingData, int32 AmountToCraft, AActor* OwningPlayer) const
{
	UPlayerInventory* PlayerInventory = InventoryUtility::GetPlayerInventory(OwningPlayer);
	if (IsValid(PlayerInventory))
	{
		return PlayerInventory->GetOwnerGold() > AmountToCraft * (CraftingData->CraftingCost * CraftingCostMultiplier);
	}
	return true;
}

bool UCraftingCore::IsCraftableItemInQueue(FCraftingData* CraftingData)
{
	if (InventoryUtility::IsCraftingDataValid(CraftingData))
	{
		return GetItemFromQueueByID(CraftingData->CraftingID).Get<0>();
	}
	return false;
}

int32 UCraftingCore::GetItemQueueIndex(FString CraftingID)
{
	for (int i = 0; i < CraftingQueue.Num(); ++i)
	{
		if (CraftingQueue[i]->CraftingID == CraftingID)
		{
			return i;
		}
	}
	return -1;
}

bool UCraftingCore::CanBeAddedToQueue() const
{
	return CraftingQueue.Num() < MaxQueuedItems;
}

TTuple<bool, FText> UCraftingCore::CanInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft,
                                                                AActor* OwningPlayer)
{
	if (bCanCraftItems)
	{
		if (CanBeAddedToQueue())
		{
			if (!IsCurrentlyCrafted(CraftingData))
			{
				if (HasPlayerEnoughGold(CraftingData, AmountToCraft, OwningPlayer))
				{
					return MakeTuple(true, FText::FromString(""));
				}
				return MakeTuple(false, FText::FromString("Not enough Gold."));
			}
			return MakeTuple(false, FText::FromString("Item is currently being Crafted"));
		}
		return MakeTuple(false, FText::FromString("Queue is full."));
	}
	return MakeTuple(false, FText::FromString("Item cannot be Crafted."));
}
