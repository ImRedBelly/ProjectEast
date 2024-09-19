#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingCore.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UCraftingCore : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshed);
	
public:
	UCraftingCore();

	virtual void BeginPlay() override;
	void ItemCrafted(AActor* OwningPlayer);
	void TryToCraftCurrentItem();
	void SpawnCraftedItem(FCraftingData* CraftingData, AActor* OwningPlayer);
	void FailedToInitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft);

	void InitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft);
	TArray<FCraftingData*>  FilterByRarity(TArray<FCraftingData*> Data);
	void CreateCraftingList();

	bool TryToStartCraftingProcess();
	bool CanStartCraftingProcess(FCraftingData* CraftingData);
	bool StartCraftingProcess(FCraftingData* CraftingData);
	void StopCraftingProcess(FCraftingData* CraftingData, AActor* OwningPlayer);
	void FinishCraftingProcess();

	TTuple<float, float> GetCraftingProcessTime();
	int32 GetCraftingQueueLength() const {return CraftingQueue.Num();}
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
	int32 GetMaxQueuedItems() const {return MaxQueuedItems;}
	bool GetCanCraftItems() const {return CanCraftItems;}

	TTuple<bool, FText> CanInitializeCraftingProcess(FCraftingData* CraftingData, uint8 AmountToCraft,
	                                                 AActor* OwningPlayer);
	bool GetIsShowLockedItems() const { return bIsShowLockedItems; }

	
	FOnRefreshed OnRefreshed;
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<UDataTable*> CraftingDataTables;
	UPROPERTY(EditAnywhere)
	TArray<FDataTableRowHandle> SingleDTItems;

	UPROPERTY(EditAnywhere)
	bool bIsShowLockedItems;
	UPROPERTY(EditAnywhere)
	int32 MaxQueuedItems;
	UPROPERTY(EditAnywhere)
	bool CanCraftItems;

private:
	TArray<FCraftingData*> CraftingList;
	TArray<FCraftingData*> CraftingQueue;
	
	ECraftingStation CraftingStation;

	FTimerHandle CraftingTimer;

	float CraftingCostMultiplier;
	float CraftingDurationRate;

	bool CraftOnlyWhenWindowIsOpen;
	bool bCanCraftItems;
	bool bSpawnCraftedItem;
	bool bIsCrafting;
};
