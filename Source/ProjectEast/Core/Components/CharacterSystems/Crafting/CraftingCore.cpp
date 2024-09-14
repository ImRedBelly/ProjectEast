#include "CraftingCore.h"

UCraftingCore::UCraftingCore()
{

}

void UCraftingCore::BeginPlay()
{
	Super::BeginPlay();
}

void UCraftingCore::ItemCrafted(AActor* OwningPlayer)
{
}

void UCraftingCore::TryToCraftCurrentItem()
{
}

void UCraftingCore::SpawnCraftedItem(FCraftingData* CraftingData, AActor* OwningPlayer)
{
}

void UCraftingCore::FailedToInitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft)
{
}

void UCraftingCore::InitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft)
{
}

void UCraftingCore::FilterByRarity(TArray<FCraftingData*> Data)
{
}

void UCraftingCore::CreateCraftingList()
{
}

bool UCraftingCore::TryToStartCraftingProcess()
{
	return false;
}

bool UCraftingCore::CanStartCraftingProcess(FCraftingData* CraftingData)
{
	return false;
}

bool UCraftingCore::StartCraftingProcess(FCraftingData* CraftingData)
{
	return false;
}

void UCraftingCore::StopCraftingProcess(FCraftingData* CraftingData, AActor* OwningPlayer)
{
}

void UCraftingCore::FinishCraftingProcess()
{
}

TTuple<float, float> UCraftingCore::GetCraftingProcessTime()
{
	return MakeTuple(0,0);
}

void UCraftingCore::AddToCraftingQueue(TArray<FCraftingData*> AddData)
{
}

void UCraftingCore::RemoveFromCraftingQueue(FCraftingData* RemoveData)
{
}

void UCraftingCore::ClearCraftingQueue(AActor* OwningPlayer)
{
}

FCraftingData* UCraftingCore::GetItemFromQueueByID()
{
	FCraftingData* CraftingData = new FCraftingData(); 
	return CraftingData;
}

FCraftingData* UCraftingCore::GetFirstItemFromQueue()
{
	FCraftingData* CraftingData = new FCraftingData(); 
	return CraftingData;
}

FCraftingData* UCraftingCore::ModifyCraftingCounter(FString CraftingID, uint8 CraftingCounter, uint8 MaxCount)
{
	FCraftingData* CraftingData = new FCraftingData(); 
	return CraftingData;
}

TArray<FCraftingData*> UCraftingCore::GetCraftingListArray()
{
	return TArray<FCraftingData*>();
}

void UCraftingCore::StartCraftingTimer(float StartTimer)
{
}

bool UCraftingCore::IsCurrentlyCrafted(FCraftingData* CraftingData)
{
	return false;
}

void UCraftingCore::DecreaseCraftingCounter(FCraftingData* CraftingData)
{
}

bool UCraftingCore::HasPlayerEnoughGold(FCraftingData* CraftingData, uint8 AmountToCraft, AActor* OwningPlayer)
{
	return false;
}

bool UCraftingCore::IsCraftableItemInQueue(FCraftingData* CraftingData)
{
	return false;
}

uint8 UCraftingCore::GetItemQueueIndex(FString CraftingID)
{
	return 0;
}

bool UCraftingCore::CanBeAddedToQueue()
{
	return false;
}

TTuple<bool, FText> UCraftingCore::CanInitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft,
	AActor* OwningPlayer)
{
	return MakeTuple(false, FText());
}


