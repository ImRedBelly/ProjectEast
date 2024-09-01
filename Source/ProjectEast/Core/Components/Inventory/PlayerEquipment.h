#pragma once

#include "CoreMinimal.h"
#include "InventoryCore.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Actors/Inventory/BaseEquippable.h"
#include "ProjectEast/Core/Components/PlayerLeveling.h"
#include "ProjectEast/Core/Utils/Structurs/FSingleDTItem.h"
#include "PlayerEquipment.generated.h"

class UPlayerInventory;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerEquipment : public UActorComponent
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddedToEquipment, FItemData&, ItemData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAttach, FItemData&, ItemData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDetach, FItemData&, ItemData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemovedFromEquipment, FItemData&, ItemData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateEquipmentMesh, EItemSlot, Slot, USkeletalMesh*, Mesh);

	GENERATED_BODY()

public:
	FOnItemAttach OnItemAttach;
	FOnItemDetach OnItemDetach;
	FOnAddedToEquipment OnAddedToEquipment;
	FOnRemovedFromEquipment OnRemovedFromEquipment;
	FOnUpdateEquipmentMesh OnUpdateEquipmentMesh;

	 UPROPERTY(EditAnywhere)
	 TArray<FSingleDTItem> InitialEquipment;

	UPROPERTY(EditAnywhere)
	bool bIsEnableOffHand;
	UPROPERTY(EditAnywhere)
	bool bIsCheckRequiredLevel;

#pragma region InitializeEquipmentEvents

	void InitializeEquipment();
	void ClientInitializeEquipment();
	void BuildEquipment();
	void BuildInitialEquipment();

#pragma endregion InitializeEquipmentEvents

#pragma region NetworkReplicationEvents

	void ServerSpawnEquippableActor(FItemData* ItemData);
	void ServerDestroyEquippableActor(FItemData* ItemData);
	void ClientUpdateAddedItem(FItemData* ItemData);
	void ClientUpdateRemovedItem(FItemData* ItemData);
	void ServerRemoveItemQuantity(FItemData* ItemData, int16 Quantity);
	void ServerTransferItemFromInventory(FItemData* ItemData, FItemData* InItemData, UInventoryCore* Inventory,
	                                     EInputMethodType Method);
	void ServerTransferItemFromEquipment(FItemData* ItemData, FItemData* InItemData);
	void ClientTransferItemReturnValue(bool Success, FText FailureMessage) const;
	void ServerUpdateEquipmentMesh(EItemSlot Slot, USkeletalMesh* Mesh) const;
	void ServerToggleEquippableSocket(bool IsInCombat, EItemSlot Slot);
	void ServerToggleAllEquippableSockets(bool IsInCombat);

#pragma endregion NetworkReplicationEvents

#pragma region NetworkReplication

	void UpdateAddedItem(FItemData* ItemData);
	void UpdateRemovedItem(FItemData* ItemData);

#pragma endregion NetworkReplication

	void AddItemToEquipmentArray(FItemData* ItemData, EItemSlot Slot);
	void RemoveItemFromEquipmentArray(FItemData* ItemData);
	void AddToStackInEquipment(FItemData* ItemData, EItemSlot Slot);
	bool TryToAddToPartialStack(FItemData* ItemData);
	void AttachItemToEquipment(FItemData* ItemData);
	void DetachItemFromEquipment(FItemData* ItemData);
	void RemoveItemQuantity(FItemData* ItemData, int32 Quantity);
	TTuple<bool, FText> TransferItemFromInventoryToEquipment(FItemData* ItemData, FItemData* InSlotData,
	                                                         UInventoryCore* Inventory, EInputMethodType Method);
	void TransferItemFromEquipmentToEquipment(FItemData* DraggedData, FItemData* InSlotData);
	TTuple<bool, FText> CanItemBeEquipped(FItemData* ItemData) const;
	TTuple<bool, FItemData*> GetItemByEquipmentSlot(EItemSlot Slot) const;
	TTuple<bool, FItemData*> GetItemByID(FString ItemID) const;
	TArray<FItemData*> GetEquipmentItems() const;
	ABaseEquippable* GetEquippedActorReference(EItemSlot Slot) const;
	FItemData* AssignItemFromEquipmentSlot(FItemData* ItemData);
	void AttachEquippableToNewSocket(EItemSlot Slot, FName SocketName) const;
	void ToggleEquippableSocket(bool CombatMode, EItemSlot Slot) const;
	void SpawnAndAttachEquippableActor(FItemData* ItemData);
	void DestroyEquippableActor(FItemData* ItemData);
	void TryToUnequipAssociatedSlot(FItemData* ItemData, UInventoryCore* Inventory);

	TMap<EItemSlot, FItemData*> GetEquipmentData() { return EquipmentData; }


	TMap<EItemSlot, ABaseEquippable*> EquippableReferences;
	TMap<EItemSlot, FItemData*> EquipmentData;

private:
	UPlayerLeveling* ActorLeveling;


	TTuple<bool, FItemData*> AAA;
};
