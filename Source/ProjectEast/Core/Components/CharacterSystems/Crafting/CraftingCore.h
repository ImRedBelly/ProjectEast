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
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClearedQueue);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingProcessStarted, FCraftingData&, CraftingData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingProcessStopped, FCraftingData&, CraftingData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingProcessFinished, FCraftingData&, CraftingData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemCrafted, FCraftingData&, CraftingData, AActor*, OwnningPlayer);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAddedToQueue, FCraftingData&, CraftingData, int32, Quantity);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRemoveFromQueue, FCraftingData&, CraftingData, int32, Quantity);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFailedToInitialize, FCraftingData&, CraftingData, int32, Quantity);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnCraftedItem, FCraftingData&, CraftingData, AActor*, OwnningPlayer);

public:
	FOnRefreshed OnRefreshed;
	FOnClearedQueue OnClearedQueue;
	FOnCraftingProcessStarted OnCraftingProcessStarted;
	FOnCraftingProcessStopped OnCraftingProcessStopped;
	FOnCraftingProcessFinished OnCraftingProcessFinished;
	FOnItemCrafted OnItemCrafted;
	FOnAddedToQueue OnAddedToQueue;
	FOnRemoveFromQueue OnRemoveFromQueue;
	FOnFailedToInitialize OnFailedToInitialize;
	FOnSpawnCraftedItem OnSpawnCraftedItem;

	virtual void BeginPlay() override;

	void ItemCrafted(AActor* OwningPlayer);
	void TryToCraftCurrentItem();
	void SpawnCraftedItem(FCraftingData* CraftingData, AActor* OwningPlayer) const;
	void FailedToInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft) const;
	void InitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft);
	void CreateCraftingList();
	void StopCraftingProcess(FCraftingData* CraftingData, AActor* OwningPlayer);
	void FinishCraftingProcess();
	void AddToCraftingQueue(FCraftingData* AddData);
	void RemoveFromCraftingQueue(FCraftingData* RemoveData);
	void ClearCraftingQueue(AActor* OwningPlayer);
	void StartCraftingTimer(float CraftingTime);
	void DecreaseCraftingCounter(FCraftingData* CraftingData);

	FCraftingData* GetFirstItemFromQueue();
	FCraftingData* ModifyCraftingCounter(FString CraftingID, int32 CraftingCounter, int32 MaxCount);

	TArray<FCraftingData*> GetCraftingListArray();
	static TArray<FCraftingData*> FilterByRarity(TArray<FCraftingData*> CraftingData);

	TTuple<bool, FCraftingData*> GetItemFromQueueByID(FString ID);
	TTuple<float, float> GetCraftingProcessTime() const;
	TTuple<bool, FText> CanInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft,
	                                                 AActor* OwningPlayer);

	bool IsCrafting() const;
	bool CanBeAddedToQueue() const;
	bool TryToStartCraftingProcess();
	bool IsCurrentlyCrafted(FCraftingData* CraftingData);
	bool StartCraftingProcess(FCraftingData* CraftingData);
	bool IsCraftableItemInQueue(FCraftingData* CraftingData);
	bool CanStartCraftingProcess(FCraftingData* CraftingData) const;
	bool HasPlayerEnoughGold(FCraftingData* CraftingData, int32 AmountToCraft, AActor* OwningPlayer) const;

	int32 GetItemQueueIndex(FString CraftingID);
	int32 GetCraftingQueueLength() const { return CraftingQueue.Num(); }

	bool GetCraftOnlyWhenWindowIsOpen() const { return CraftOnlyWhenWindowIsOpen; }
	bool GetIsShowLockedItems() const { return bIsShowLockedItems; }
	bool GetCanCraftItems() const { return bCanCraftItems; }
	bool GetSpawnCraftedItem() const { return bSpawnCraftedItem; }
	int32 GetMaxQueuedItems() const { return MaxQueuedItems; }
	float GetCraftingCostMultiplier() const { return CraftingCostMultiplier; }

protected:
	UPROPERTY(EditAnywhere)
	TArray<UDataTable*> CraftingDataTables;
	UPROPERTY(EditAnywhere)
	TArray<FDataTableRowHandle> SingleDTItems;

	UPROPERTY(EditAnywhere)
	ECraftingStation CraftingStation;

	UPROPERTY(EditAnywhere)
	bool bSpawnCraftedItem;
	UPROPERTY(EditAnywhere)
	bool bIsShowLockedItems;
	UPROPERTY(EditAnywhere)
	bool CraftOnlyWhenWindowIsOpen;
	UPROPERTY(EditAnywhere)
	bool bCanCraftItems;

	UPROPERTY(EditAnywhere)
	float CraftingCostMultiplier = 1;

	UPROPERTY(EditAnywhere)
	float CraftingDurationRate;

	UPROPERTY(EditAnywhere)
	int32 MaxQueuedItems;

	UPROPERTY(EditAnywhere)
	FCraftingData EmptyCraftingData;

private:
	TArray<FCraftingData*> CraftingList;
	TArray<FCraftingData*> CraftingQueue;

	FTimerHandle CraftingTimer;
	bool bIsCrafting;
};
