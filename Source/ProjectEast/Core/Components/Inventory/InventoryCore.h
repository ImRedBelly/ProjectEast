#pragma once

#include "CoreMinimal.h"
#include "PlayerEquipment.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Actors/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "InventoryCore.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FString ID;
	UPROPERTY(EditDefaultsOnly)
	EItemSlot ItemSlot;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainItemData> Class;
	UPROPERTY(EditDefaultsOnly)
	int32 Quantity;
	UPROPERTY(EditDefaultsOnly)
	int32 Durability;
	UPROPERTY(EditDefaultsOnly)
	int32 Index;
	UPROPERTY(EditDefaultsOnly)
	bool bIsEquipped;
	UPROPERTY(EditDefaultsOnly)
	bool bIsAlreadyUsed;
	UPROPERTY(EditDefaultsOnly)
	float ValueModifier;
};

USTRUCT(BlueprintType)
struct FSingleDTItem
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle TableAndRow;
	UPROPERTY(EditDefaultsOnly)
	int32 Quantity;
};

USTRUCT()
struct FRandomizedLootTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UDataTable* DataTable;
	
	UPROPERTY(EditAnywhere)
	int32 MinLootItems;
	
	UPROPERTY(EditAnywhere)
	int32 MaxLootItems;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshInventory, EInventoryPanels, Panel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHighlightInventorySlot, int32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedActivePanel, EInventoryPanels, Panel);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UInventoryCore : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnRefreshInventory OnRefreshInventory;
	FOnHighlightInventorySlot OnHighlightInventorySlot;
	FOnSwitchedActivePanel OnSwitchedActivePanel;

	virtual void InitializeInventory(APlayerController* PlayerController);
	void CallOnRefreshInventory(EInventoryPanels Panel) const;

	//Client

	void ClientTransferItemReturnValue(bool Success, FText FailureMessage);
	void ClientUpdateItems(UInventoryCore* Inventory, EInventoryPanels InventoryPanel, TArray<FItemData*> Array);
	void ClientUpdateAddedItem(FItemData* ItemData, UInventoryCore* Inventory);
	void ClientUpdateRemovedItem(FItemData* ItemData, UInventoryCore* Inventory);


	void SortInventory(ESortMethod Method, EInventoryPanels SinglePanel, bool EveryPanel);
	void UpdateViewInventory(EInventoryPanels InventoryPanel);

	//Server
	void ServerUpdateItems(AActor* Actor);
	void MulticastSetCurrentWeight(float CurrentWeight);
	void MulticastSetOwnerGold(float OwnerGold);
	void AddViewer(APlayerState* PlayerState, UInventoryCore* Inventory);
	void RemoveViewer(APlayerState* PlayerState, UInventoryCore* Inventory);

	void ServerMoveItemToSlot(UInventoryCore Inventory, EInventoryPanels InventoryPanels, int32 MoveFrom, int32 MoveTo);
	void ServerAddItemToInventory(UInventoryCore Inventory, FItemData ItemData, int32 SlotIndex);
	void ServerRemoveItemFromInventory(UInventoryCore Inventory, FItemData ItemData);
	void ServerTransferItemFromInventory(UInventoryCore Receiver, FItemData ItemData, FItemData InSlotData,EInputMethodType Method, UInventoryCore Sender, AActor* OwningPlayer);
	void ServerTransferItemFromEquipment(FItemData ItemData, FItemData InSlotData, EInputMethodType Method,UPlayerEquipment Sender);
	void ServerSplitItemsInInventory(UInventoryCore Receiver, UInventoryCore Sender, FItemData ItemData,
	                                 FItemData InSlotData, FItemData StackableLeft, EInputMethodType Method,
	                                 EInputMethodType Initiator, EInputMethodType Destination, AActor* OwningPlayer);
	void ServerConfirmationPopupAccepted(UInventoryCore Receiver, UInventoryCore Sender, FItemData ItemData,
	                                     FItemData InSlotData, EInputMethodType Method, EInputMethodType Initiator,
	                                     EInputMethodType Destination, AActor* OwningPlayer);
	void ServerSortInventory(UInventoryCore Inventory, EInputMethodType Method, EInventoryPanels SinglePanel,
	                         bool EveryPanel);

	
	TTuple<TArray<FItemData*>, int32>GetInventoryAndSize(EInventoryPanels Panel);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESortMethod DefaultSortingMethod;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FSingleDTItem> SingleDTItem;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EInventoryPanels> PanelsToUse;
	
	virtual void BeginPlay() override;

	int32 CurrentInventorySize;
	EInventoryPanels CurrentPanel;
	EInventoryPanels ActivePanel;
	TArray<FItemData*> CurrentInventory;
	TArray<FItemData*> CurrentSortedItems;

	TArray<FItemData*> CurrentInventoryArray;
	TArray<FItemData*> CurrentEmptyInventoryArray;

	FItemData* CurrentItemData;
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

	bool bIsUseInventorySize;
	bool bSortInitialItems;


	void BuildInitialInventory();
	void BuildInventory(EInventoryPanels Panel);
	void ApplyChangesToInventoryArray(EInventoryPanels Panel, TArray<FItemData*> Inventory);


	void RandomizeInitialItems();

	TTuple<bool, int32> GetEmptyInventorySlot(const FItemData* ItemData);

	void AddItemToInventoryArray(FItemData* ItemData, int32 Index);
	void AddWeightToInventory(float Weight);
	void ModifyItemValue(FItemData* ItemData);
	void SwitchActivePanel(EInventoryPanels Panel);
	void UpdateViewItem(FItemData* ItemData, bool IsRemove);
};
