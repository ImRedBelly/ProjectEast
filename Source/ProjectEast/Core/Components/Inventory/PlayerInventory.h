#pragma once

#include "CoreMinimal.h"
#include "InventoryCore.h"
#include "PlayerInventory.generated.h"


class UInteractableComponent;
class UInventoryWindow;
class ALootBag;
class UStorageWindow;
class IInteractable;
class AMainPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeAllItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemLooted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, FItemData, ItemData);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerInventory : public UInventoryCore
{
	GENERATED_BODY()

public:
	virtual void InitializeInventory(APlayerController* PlayerController) override;
	virtual void BeginPlay() override;
	virtual void ClientTransferItemReturnValue(bool Success, FText FailureMessage) override;
	void ClientInitializeInventory(APlayerController* PlayerController);
	void InputInteraction() const;
	void ServerTakeAllItems(UInventoryCore* Sender, AActor* OwningPlayer);
	void InputCloseWidget();
	void InputInventory();

	void OpenLootBarWidget();
	void CloseLootBarWidget();

	void CloseInventoryWidget();

	void OpenStorageWidget();
	void CloseStorageWidget();

	void OpenVendorWidget();
	void CloseVendorWidget();

	void ServerChangeDroppedIndex(uint32 DroppedIndex);
	void ServerTakeItem(FItemData* ItemData, UInventoryCore* Sender, AActor* OwningPlayer);
	void ServerDropItemOnTheGround(FItemData* ItemData, EItemDestination Initiator, AActor* OwningPlayer);
	
	bool IsRefreshOmClosingWidget() const { return bIsRefreshOmClosingWidget; }

	FOnTakeItem OnTakeItem;
	FOnTakeAllItems OnTakeAllItems;
	FOnItemLooted OnItemLooted;
	FOnItemUsed OnItemUsed;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsRefreshOmClosingWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsInteractableActorWidgetOpen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventoryWindow> DefaultInventoryWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UStorageWindow> DefaultStorageWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ALootBag> DefaultLootBag;

	
private:
	void InputCloseWidget() const;
	void InputTakeAll() const;


	void ServerSpawnLootBag(FItemData* ItemData, AActor* OwningPlayer);
	void ServerSpawnInteractable(FItemData* ItemData, AActor* OwningPlayer);
	void ServerModifyItemDurability(FItemData* ItemData, uint32 Amount, AActor* OwningPlayer);

	void ClientItemLooted(FItemData* ItemData);
	void ClientTakeItemReturnValue(bool Success, FText FailureMessage, bool RemoveInteraction) const;

	void TakeItem(FItemData* ItemData, UInventoryCore* Sender, AActor* OwningPlayer);
	void TakeAllItems();
	void DropItemOnTheGround(FItemData* ItemData, EItemDestination Initiator, AActor* OwningPlayer);
	void SpawnLootBagNearThePlayer(FItemData* ItemData, AActor* OwningPlayer);
	void SpawnItemMeshNearThePlayer();
	ALootBag* IsCollidingWithLootBag(FVector StartPosition, FVector EndPosition) const;

	virtual TTuple<bool, FText> TransferItemFromInventory(FItemData* ItemData, FItemData* IsSlotData,
	EInputMethodType InputMethod, UInventoryCore* Sender, AActor* OwningPlayer) override;
	virtual void SplitItemsInInventory(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
	FItemData* StackableLeft, EInputMethodType Method, EItemDestination Initiator,
	EItemDestination Destination, AActor* OwningPlayer) override;
	virtual void ConfirmationPopupAccepted(UInventoryCore* Sender, FItemData* ItemData, FItemData* InSlotData,
										   EInputMethodType Method,
										   EInputMethodType Initiator, EInputMethodType Destination,
										   AActor* OwningPlayer) override;
	bool AttemptUsingTransferredItem(FItemData* ItemData, UInventoryCore* Sender);
	virtual void AddItemToInventoryArray(FItemData* ItemData, int32 SlotIndex) override;
	virtual void SwapItemsInInventory(FItemData* FirstItem, FItemData* SecondItem) override;
	
	AMainPlayerController* CashedPlayerController;
	UInventoryWindow* CashedInventoryWindow;
	UStorageWindow* CashedStorageWindow;


	UInteractableComponent* GetCurrentInteractable() const;

	ALootBag* CachedLootBag; 
	uint32 MainDroppedIndex;
	bool bIsLootBarOpen;
};
