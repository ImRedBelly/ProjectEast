#pragma once

#include "CoreMinimal.h"
#include "SortWindow.h"
#include "PlayerInventorySlot.h"
#include "InventoryPanelButton.h"
#include "Components/WidgetSwitcher.h"
#include "Components/UniformGridPanel.h"
#include "PlayerInventoryWidget.generated.h"


UCLASS()
class PROJECTEAST_API UPlayerInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AssignCurrentlyFocusedSlot(UPlayerInventorySlot* PlayerInventorySlot);
	void ScrollToSlot(UPlayerInventorySlot* PlayerInventorySlot) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UPlayerInventorySlot> DefaultPlayerInventorySlot;
	
	UPROPERTY(meta=(BindWidget))
	UBorder* BackgroundBorder;

	UPROPERTY(meta=(BindWidget))
	UImage* BackgroundImage;

	UPROPERTY(meta=(BindWidget))
	UInventoryPanelButton* ButtonPanel1;

	UPROPERTY(meta=(BindWidget))
	UInventoryPanelButton* ButtonPanel2;

	UPROPERTY(meta=(BindWidget))
	UInventoryPanelButton* ButtonPanel3;

	UPROPERTY(meta=(BindWidget))
	UInventoryPanelButton* ButtonPanel4;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* InventoryUG1;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* InventoryUG2;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* InventoryUG3;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* InventoryUG4;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* PanelSwitcher;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* ScrollBox;

	UPROPERTY(meta=(BindWidget))
	USortWindow* SortWindow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D DraggedImageSize;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PanelName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString P1Title = "ARMOR & WEAPONS";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString P2Title = "CONSUMABLES";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString P3Title = "CRAFTING";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString P4Title = "QUEST ITEMS";

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationConstruct;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void BindEventDispatchers();
	void UnbindEventDispatchers();
	
	UFUNCTION()
	void RefreshInventory(EInventoryPanels Panel);
	
	UFUNCTION()
	void HighlightSlot(uint32 SlotIndex);
	
	UFUNCTION()
	void SwitchInventoryPanel(EInventoryPanels NewPanel);
	
	UFUNCTION()
	void OnSortingFinished();
	
	void SetActivePanel();
	void SetReceiverInventory();
	void SetInitialInputDelayForSlot() const;
	void ResetSlotFocus();
	void CreateInventoryP1();
	void CreateInventoryP2();
	void CreateInventoryP3();
	void CreateInventoryP4();
	void SetFocusToSlot(uint32 SlotIndex) const;
	void BuildInventorySlots(TArray<FItemData*> ItemData, int32 Size, UUniformGridPanel* GridPanel);
	EInventoryPanels GetActivePanel() const;
	UUniformGridPanel* GetUniformGridFromPanel(EInventoryPanels Panel) const;
	void SwitchToNextInventoryPanel(bool ToTheRight);
	void SetPanelTitle(EInventoryPanels Panel);
	void DisplaySampleSlots(int32 IndexSlot);
	bool IsUsingGamepad() const;
	bool IsAnyPopupActive() const;

private:
	UInventoryCore* CachedReceiverInventory;
	UPlayerInventory* CachedPlayerInventory;
	UPlayerEquipment* CachedPlayerEquipment;
	
	AMainPlayerController* CachedPlayerController;

	uint32 ColumnPosition;
	uint32 RowPosition;
	uint32 RowLength = 5;
	uint32 FocusedSlot;
	uint32 AAA;
	uint32 SampleSlotsToDisplay = 30;
	
	EInventoryPanels DefaultPanel = EInventoryPanels::P1;
};
