#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Utils/Structurs/FCraftingData.h"
#include "CraftingCore.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UCraftingCore : public UActorComponent
{
	GENERATED_BODY()
protected:
	UCraftingCore();
	
	virtual void BeginPlay() override;
	void ItemCrafted(AActor* OwningPlayer);
	void TryToCraftCurrentItem();
	void SpawnCraftedItem(FCraftingData* CraftingData, AActor* OwningPlayer);
	void FailedToInitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft);

	void InitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft);
	void FilterByRarity(TArray<FCraftingData*> Data);
	
	bool TryToStartCraftingProcess();
	bool CanStartCraftingProcess(FCraftingData* CraftingData);
	bool StartCraftingProcess(FCraftingData* CraftingData);
	void StopCraftingProcess(FCraftingData* CraftingData, AActor* OwningPlayer);
	void FinishCraftingProcess();
	
	TTuple<float, float> GetCraftingProcessTime();
	void AddToCraftingQueue(TArray<FCraftingData*> AddData);
	void RemoveFromCraftingQueue(FCraftingData* RemoveData);
	void ClearCraftingQueue(AActor* OwningPlayer);
	
	FCraftingData* GetItemFromQueueByID();
	FCraftingData* GetFirstItemFromQueue();
	FCraftingData* ModifyCraftingCounter(FString CraftingID, uint8 CraftingCounter, uint8 MaxCount);
	TArray<FCraftingData*> GetCraftingListArray();
	void StartCraftingTimer(float StartTimer);
	
	bool IsCurrentlyCrafted(FCraftingData* CraftingData);
	void DecreaseCraftingCounter(FCraftingData* CraftingData);
	bool HasPlayerEnoughGold(FCraftingData* CraftingData, uint8 AmountToCraft, AActor* OwningPlayer);
	bool IsCraftableItemInQueue(FCraftingData* CraftingData);
	uint8 GetItemQueueIndex(FString CraftingID);
	bool CanBeAddedToQueue();
	
	TTuple<bool, FText>  CanInitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft, AActor* OwningPlayer);
	
};
