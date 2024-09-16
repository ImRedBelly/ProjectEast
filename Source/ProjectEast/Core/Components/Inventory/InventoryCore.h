#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "InventoryCore.generated.h"

class UPlayerEquipment;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshInventory, EInventoryPanels, Panel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemovedFromInventoryArray, FItemData&, ItemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddedToInventoryArray, FItemData&, ItemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHighlightInventorySlot, uint32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedActivePanel, EInventoryPanels, Panel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedOwnerGold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedCurrentWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedMaxWeight);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UInventoryCore : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnRefreshInventory OnRefreshInventory;
	FOnRemovedFromInventoryArray OnRemovedFromInventoryArray;
	FOnAddedToInventoryArray OnAddedToInventoryArray;
	FOnHighlightInventorySlot OnHighlightInventorySlot;
	FOnSwitchedActivePanel OnSwitchedActivePanel;
	FOnChangedOwnerGold OnChangedOwnerGold;
	FOnChangedCurrentWeight OnChangedCurrentWeight;
	FOnChangedMaxWeight OnChangedMaxWeight;

	virtual void InitializeInventory(APlayerController* PlayerController);
	virtual void BeginPlay() override;

	void AddViewer(APlayerState* PlayerState, UInventoryCore* Inventory);
	void RemoveViewer(APlayerState* PlayerState, UInventoryCore* Inventory);
	void RemoveItemFromInventoryArray(FItemData* ItemData);
	void RemoveWeightFromInventory(float Weight);


	void MulticastSetCurrentWeight(float CurrentWeight);
	void MulticastSetOwnerGold(float Gold);


	//Client
	void ClientUpdateItems(UInventoryCore* Inventory, EInventoryPanels InventoryPanel, TArray<FItemData*> Array) const;
	virtual void ClientTransferItemReturnValue(bool Success, FText FailureMessage);
	void ClientUpdateAddedItem(FItemData* ItemData, UInventoryCore* Inventory);
	void ClientUpdateRemovedItem(FItemData* ItemData, UInventoryCore* Inventory);

	void SortInventory(ESortMethod Method, EInventoryPanels SinglePanel, bool EveryPanel);

	//Server
	static void ServerRemoveItemFromInventory(UInventoryCore* Inventory, FItemData* ItemData);
	static void ServerAddItemToInventory(UInventoryCore* Inventory, FItemData* ItemData, int32 SlotIndex);
	void ServerUpdateItems(AActor* Actor);
	void ServerTransferItemFromInventory(UInventoryCore* Receiver, FItemData* ItemData, FItemData* InSlotData,
	                                     EInputMethodType Method, UInventoryCore* Sender, AActor* OwningPlayer);
	static void ServerMoveItemToSlot(UInventoryCore* Inventory, EInventoryPanels Panel, int32 MoveFrom, int32 MoveTo);
	static void ServerSplitItemsInInventory(UInventoryCore* Receiver, UInventoryCore* Sender, FItemData* ItemData,
	                                        FItemData* InSlotData, FItemData* StackableLeft, EInputMethodType Method,
	                                        EItemDestination Initiator, EItemDestination Destination,
	                                        AActor* OwningPlayer);
	static void ServerSortInventory(UInventoryCore* Inventory, ESortMethod Method, EInventoryPanels SinglePanel,
	                                bool EveryPanel);
	void ServerTransferItemFromEquipment(FItemData* ItemData, FItemData* InSlotData, EInputMethodType Method,
	                                     UPlayerEquipment* Sender);
	static void ServerConfirmationPopupAccepted(UInventoryCore* Receiver, UInventoryCore* Sender, FItemData* ItemData,
	                                            FItemData* InSlotData, EInputMethodType Method,
	                                            EInputMethodType Initiator,
	                                            EInputMethodType Destination, AActor* OwningPlayer);
	//Network Replication
	void UpdateViewersInventory(EInventoryPanels Panel);
	void UpdateAddedItemInInventory(FItemData* ItemData, UInventoryCore* Inventory);
	void UpdateViewersItem(FItemData* ItemData, bool Remove);
	void UpdateRemovedItemInInventory(FItemData* ItemData, UInventoryCore* Inventory);

	void CallOnRefreshInventory(EInventoryPanels Panel) const;
	void CallOnAddedToInventorySlot(FItemData& ItemData) const;
	void CallOnRemovedFromInventoryArray(FItemData& ItemData) const;
	void CallOnHighlightSlot(uint32 Index) const;
	EInventoryPanels GetActivePanel() const;
	TTuple<TArray<FItemData*>, int32> GetInventoryAndSize(EInventoryPanels Panel);
	void SwitchedActivePanel(EInventoryPanels Panel);
	EItemRemoveType GetItemRemoveType(FItemData* ItemData) const;
	void AddGoldToOwner(float Gold);
	void AddToStackInInventory(FItemData* ItemData, int32 Index);
	virtual void AddItemToInventoryArray(FItemData* ItemData, int32 SlotIndex);
	void AddWeightToInventory(float Weight);

	float GetOwnerGold() const { return OwnerGold; }
	bool CheckOwnerGold() const { return bIsCheckOwnerGold; }
	bool IsRefreshOnClosingWidget() const { return bIsRefreshOnClosingWidget; }
	TTuple<bool, FText> TransferItemFromEquipment(FItemData* ItemData, FItemData* IsSlotData,EInputMethodType InputMethod,UPlayerEquipment* Sender);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESortMethod DefaultSortingMethod;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FSingleDTItem> SingleDTItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EInventoryPanels> PanelsToUse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsUseInventoryWeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ValueModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxInventoryWeight = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsCheckOwnerGold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsRefreshOnClosingWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsUseInventorySize = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString MessageNotEnoughGold = "Not enough gold.";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString MessageInventoryFull = "Inventory is full.";

	EInventoryPanels ActivePanel;
	
	TArray<UDataTable> AllItemsFromDT;
	TArray<APlayerState*> CurrentViewers;
	TArray<FRandomizedLootTable> RandomizedItemsData;

	TArray<FItemData*> InventoryP1;
	TArray<FItemData*> InventoryP2;
	TArray<FItemData*> InventoryP3;
	TArray<FItemData*> InventoryP4;

	int32 InventorySizeP1 = 30;
	int32 InventorySizeP2 = 30;
	int32 InventorySizeP3 = 30;
	int32 InventorySizeP4 = 30;

	bool bSortInitialItems;
	bool bIsCheckRemoveType;


	void BuildInventory(EInventoryPanels Panel);
	void BuildInitialInventory();
	void SortInventoryByPanel(ESortMethod Method, EInventoryPanels Panel);
	void ApplyChangesToInventoryArray(EInventoryPanels Panel, TArray<FItemData*> Inventory);
	virtual TTuple<bool, FText> TransferItemFromInventory(FItemData* ItemData, FItemData* IsSlotData,
	                                                      EInputMethodType InputMethod,
	                                                      UInventoryCore* Sender, AActor* OwningPlayer);
	virtual void SwapItemsInInventory(FItemData* FirstItem, FItemData* SecondItem);
	bool HasEnoughGold(FItemData* ItemData) const;
	void RemoveGoldFromOwner(float Gold);
	bool IsInventoryOverweight() const;
	TTuple<bool, int> GetEmptyInventorySlot(FItemData* ItemData);
	FItemData* GetItemBySlot(EInventoryPanels Panel, uint32 SlitIndex);
	TTuple<FItemData*, bool> GetItemByID(FString ID, EInventoryPanels InSpecifiedPanel);
	TArray<FItemData*> GetCombinedInventories() const;
	TTuple<FItemData*, bool> GetItemByData(FItemData* ItemData);
	TArray<FItemData*> GetItemsOfSpecifiedType(EItemsType ItemsType) const;
	uint32 GetAmountOfEmptySlots(EInventoryPanels Panels);
	void SwitchActivePanel(EInventoryPanels Panel);
	void RemoveItemQuantity(FItemData* ItemData, uint32 Quantity);
	void ModifyItemValue(FItemData* ItemData) const;
	void RandomizeInitialItems();
	FItemData* RandomizeItemParameters(FItemData* ItemData);
	virtual void SplitItemsInInventory(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
	                                   FItemData* StackableLeft,
	                                   EInputMethodType Method, EItemDestination Initiator,
	                                   EItemDestination Destination, AActor* OwningPlayer);
	virtual void ConfirmationPopupAccepted(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
	                                       EInputMethodType Method,
	                                       EInputMethodType Initiator, EInputMethodType Destination,
	                                       AActor* OwningPlayer);
	TTuple<bool, int32> GetEmptyInventorySlot(const FItemData* ItemData);

	void ChangeInventorySize(EInventoryPanels Panels, uint32 Size);
	void OnRep_OwnerGold();
	void OnRep_InventorySizeP1();
	void OnRep_InventorySizeP2();
	void OnRep_InventorySizeP3();
	void OnRep_InventorySizeP4();
	void OnRep_CurrentInventoryWeight();
	void OnRep_MaxInventoryWeight();


	void UpdateViewItem(FItemData* ItemData, bool IsRemove);

private:
	float OwnerGold;
	float CurrentInventoryWeight;

	void LocalSwapItemsInInventory(FItemData* LocalFirstItem, FItemData* LocalSecondItem);
};
