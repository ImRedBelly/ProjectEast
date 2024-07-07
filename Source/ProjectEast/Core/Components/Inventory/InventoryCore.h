#pragma once

#include "CoreMinimal.h"
#include "PlayerEquipment.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "InventoryCore.generated.h"

class AMainItem;

USTRUCT()
struct FItemData
{
	GENERATED_BODY()

	FString ID;
	
	EItemSlot ItemSlot;
	TSubclassOf<AMainItem> Class;
	int32 Quantity;
	int32 Durability;
	int32 Index;
	bool bIsEquipped;
	bool bIsAlreadyUsed;
	float ValueModifier;
};

USTRUCT()
struct FSingleDTItem
{
	GENERATED_BODY()

	FDataTableRowHandle TableAndRow;
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


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UInventoryCore : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnRefreshInventory OnRefreshInventory;

	virtual void InitializeInventory(APlayerController* PlayerController);
	void CallOnRefreshInventory(EInventoryPanels Panel) const;

	//Client

	void ClientTransferItemReturnValue(bool Success, FText FailureMessage);
	void ClientUpdateItems(UInventoryCore* Inventory, EInventoryPanels InventoryPanel, TArray<FItemData*> Array);
	void ClientUpdateAddedItem(FItemData ItemData, UInventoryCore Inventory);
	void ClientUpdateRemovedItem(FItemData ItemData, UInventoryCore Inventory);


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

	virtual void BeginPlay() override;

	TArray<EInventoryPanels> PanelsToUse;

	int32 CurrentInventorySize;
	EInventoryPanels CurrentPanel;
	TArray<FItemData*> CurrentInventory;
	TArray<FItemData*> CurrentSortedItems;

	TArray<FItemData*> CurrentInventoryArray;
	TArray<FItemData*> CurrentEmptyInventoryArray;

	FItemData* CurrentItemData;
	TArray<UDataTable> AllItemsFromDT;
	TArray<FSingleDTItem> SingleDTItem;
	TArray<FRandomizedLootTable> RandomizedItemsData;

	TArray<FItemData*> InventoryP1;
	TArray<FItemData*> InventoryP2;
	TArray<FItemData*> InventoryP3;
	TArray<FItemData*> InventoryP4;

	int32 InventorySizeP1;
	int32 InventorySizeP2;
	int32 InventorySizeP3;
	int32 InventorySizeP4;

	bool bIsUseInventorySize;
	bool bSortInitialItems;


	void BuildInitialInventory();
	void BuildInventory(EInventoryPanels Panel);
	void ApplyChangesToInventoryArray(EInventoryPanels Panel, TArray<FItemData*> Inventory);


	void RandomizeInitialItems();

	TTuple<bool, int32> GetEmptyInventorySlot(const FItemData* ItemData);

	void AddItemToInventoryArray(FItemData* ItemData, int32 Index);
	void AddWeightToInventory(float Weight);
};
