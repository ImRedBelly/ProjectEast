#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "VendorInventory.generated.h"


class UWidgetManager;
class UPlayerInventory;
class UVendorSlot;
class UInventoryCore;

UCLASS()
class PROJECTEAST_API UVendorInventory : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeVendorInventory(UPlayerInventory* InPlayerInventory, UWidgetManager* InWidgetManager);
	void AssignCurrentlyFocusedSlot(UUserWidget* ChildWidget);
	void ScrollToSlot(UVendorSlot* VendorSlot) const;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderBackground;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* InventoryUG1;
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* PanelSwitcher;
	UPROPERTY(meta=(BindWidget))
	UScrollBox* ScrollBoxSlots;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextGold;

	UPROPERTY(EditAnywhere)
	int32 SampleSlotsToDisplay;
	UPROPERTY(EditAnywhere)
	int32 RowLength;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UVendorSlot> VendorSlotClass;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ConstructAnimation;

private:
	UPROPERTY()
	UInventoryCore* OwnerInventory;
	UPROPERTY()
	UPlayerInventory* PlayerInventory;
	UPROPERTY()
	UWidgetManager* WidgetManager;

	int32 ColumnPosition;
	int32 RowPosition;
	int32 FocusedSlot;

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void RefreshInventory(EInventoryPanels Panels);
	UFUNCTION()
	void HighlightSlot(int32 Index);
	UFUNCTION()
	void SwitchInventoryPanel(EInventoryPanels Panel);
	UFUNCTION()
	void SetGoldValue();

	void BindEventDispatchers();
	void UnbindEventDispatchers();
	void BuildInventorySlots(TArray<FItemData*> ItemData, int32 Size, UUniformGridPanel* UniformGridPanel);
	void SetOwnerInventory();
	void ResetSlotFocus();
	void CreateInventoryP1();

	void SetFocusToSlot(int32 Index);
	void DisplaySampleSlots(int32 Slots);
	EInventoryPanels GetActivePanel() const;
	UUniformGridPanel* GetUniformGridFromPanel(EInventoryPanels Panel);
};
