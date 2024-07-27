#pragma once

#include "CoreMinimal.h"
#include "InventoryCore.h"
#include "Components/ActorComponent.h"
#include "PlayerEquipment.generated.h"


class UPlayerInventory;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerEquipment : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void ServerTransferItemFromInventory(FItemData* ItemData, FItemData* InSlotData, UPlayerInventory* PlayerInventory, EInputMethodType RightClick);
	void RemoveItemFromEquipmentArray(FItemData* ItemData);
	void AddToStackInEquipment(FItemData* ItemData, FItemData* Element);
	bool TryToAddToPartialStack(FItemData* ItemData);
	void DetachItemFromEquipment(FItemData* ItemData);
	bool CanItemBeEquipped(FItemData* ItemData);
	TTuple<bool, FItemData*> GetItemByEquipmentSlot(EItemSlot Slot) const;

	bool bIsEnableOffHand;
	
private:
	TMap<EItemSlot, FItemData*> EquipmentData;
};
