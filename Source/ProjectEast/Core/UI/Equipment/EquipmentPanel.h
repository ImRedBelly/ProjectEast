#pragma once

#include "CoreMinimal.h"
#include "RotatePlayerBox.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "EquipmentPanel.generated.h"

class UEquipmentSlot;

UCLASS()
class PROJECTEAST_API UEquipmentPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	void AssignCurrentlyFocusedSlot(EItemSlot ItemSlot);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* BootSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* ChestPlateSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* GlovesSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* HeadSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* LegsSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* NecklaceSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* ShieldSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* WeaponSlot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* Pot1Slot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* Pot2Slot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* Pot3Slot;
	UPROPERTY(meta=(BindWidget))
	UEquipmentSlot* Pot4Slot;
	UPROPERTY(meta=(BindWidget))
	URotatePlayerBox* RotatePlayerBox;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void EmptyEquipmentSlot(FItemData& ItemData);
	UFUNCTION()
	void OverwriteEquipmentSlot(FItemData& ItemData);
	
	void SetFocusToSlot(EItemSlot ItemSlot);
	UEquipmentSlot* GetWidgetSlotByItemSlot(EItemSlot ItemSlot) const;

	void CreateEquipment();
	void BindEventDispatchers();
	void UnbindEventDispatchers();

private:
	UPlayerEquipment* CachedPlayerEquipment;
	UPlayerInventory* CachedPlayerInventory;
	EItemSlot FocusedSlot;
};
