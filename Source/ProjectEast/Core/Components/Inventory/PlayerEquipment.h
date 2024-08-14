#pragma once

#include "CoreMinimal.h"
#include "InventoryCore.h"
#include "Components/ActorComponent.h"
#include "PlayerEquipment.generated.h"

class UPlayerInventory;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddedToEquipment, FItemData, ItemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemovedFromEquipment, FItemData, ItemData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerEquipment : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnAddedToEquipment OnAddedToEquipment;
	FOnRemovedFromEquipment OnRemovedFromEquipment;


	UPROPERTY(EditAnywhere)
	bool bIsEnableOffHand;
	UPROPERTY(EditAnywhere)
	bool bIsCheckRequiredLevel;

	virtual void BeginPlay() override;
	void BuildEquipment();

	void ServerTransferItemFromInventory(FItemData* ItemData, FItemData* InSlotData, UInventoryCore* PlayerInventory,
	                                     EInputMethodType RightClick);
	void ServerTransferItemFromEquipment(FItemData* ItemData, FItemData* CurrentItemData);
	void ServerRemoveItemQuantity(FItemData* ItemData, int8 Quantity);
	void RemoveItemFromEquipmentArray(FItemData* ItemData);
	void AddItemToEquipmentArray(FItemData* ItemData, EItemSlot ItemSlot);
	void AddToStackInEquipment(FItemData* ItemData, FItemData* Element);
	void RemoveItemQuantity(FItemData* ItemData, int8 Quantity);
	bool TryToAddToPartialStack(FItemData* ItemData);
	void DetachItemFromEquipment(FItemData* ItemData);
	
	TTuple<bool, FText> CanItemBeEquipped(FItemData* ItemData);
	TTuple<bool, FItemData*> GetItemByEquipmentSlot(EItemSlot Slot) const;
	TMap<EItemSlot, FItemData*> GetEquipmentData() { return EquipmentData; }

private:
	TMap<EItemSlot, FItemData*> EquipmentData;
};
