#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "StorageInventory.generated.h"

class UWidgetManager;
class UPlayerInventory;
class UInventoryCore;
class UInputDeviceViewComponent;
class UStorageSlot;

UCLASS()
class PROJECTEAST_API UStorageInventory : public UUserWidget
{
	GENERATED_BODY()

public:
	void AssignCurrentlyFocusedSlot(UStorageSlot* StorageSlot);
	void ScrollToSlot(UStorageSlot* StorageSlot) const;

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;

	UPROPERTY(meta=(BindWidget))
	UBorder* BorderBackground;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* InventoryUG1;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* PanelSwitcher;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* ScrollBox;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationConstruct;

	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UStorageSlot> DefaultStorageSlot;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void ResetSlotFocus();
	void CreateInventoryP1();
	void SetOwnerInventory();
	void BindEventDispatchers();
	void UnbindEventDispatchers();

	void DisplaySampleSlots(int32 IndexSlot);
	void SetFocusToSlot(uint32 SlotIndex);
	void BuildInventorySlots(TArray<FItemData*> ItemData, int32 Size, UUniformGridPanel* GridPanel);

	static EInventoryPanels GetActivePanel() { return EInventoryPanels::P1; }
	UUniformGridPanel* GetUniformGridFromPanel(EInventoryPanels Panel) const;

	UFUNCTION()
	void SwitchInventoryPanel(EInventoryPanels NewPanel);
		
	UFUNCTION()
	void HighlightSlot(uint32 SlotIndex);

	UFUNCTION()
	void RefreshInventory(EInventoryPanels Panel);

private:
	UInventoryCore* CachedOwnerInventory;
	UPlayerInventory* CachedPlayerInventory;
	UWidgetManager* WidgetManager;
	FIconButtonGameModule* IconButtonGameModule;

	uint32 ColumnPosition;
	uint32 RowPosition;
	uint32 RowLength = 5;
	uint32 FocusedSlot;
	uint32 SampleSlotsToDisplay = 30;
};
