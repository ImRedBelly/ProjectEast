﻿#pragma once
#include "CoreMinimal.h"
#include "Components/Border.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "PlayerInventorySlot.generated.h"

class UInventoryCore;
class UPlayerInventory;
class UWidgetManager;
class UPlayerEquipment;
class AMainPlayerController;
class UItemDataDragDropOperation;
class UItemDataDragAndDropPanel;
class UToolTip;
class UPlayerInventoryWidget;
class UButton;


UCLASS()
class PROJECTEAST_API UPlayerInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(AMainPlayerController* PlayerController, FItemData* ItemData, UInventoryCore* ReceiverInventory, UPlayerInventoryWidget* ParentWidget,
	FVector2D DragImageSize, uint32 IndexSlot);
	
	void HighlightSlot();
	void OnInitialInput();
	void OffInitialInput();
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonItem;

	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextQuantity;

	UPROPERTY(meta=(BindWidget))
	UBorder* BorderObject;

	UPROPERTY(EditAnywhere)
	FLinearColor BorderHovered;

	UPROPERTY(EditAnywhere)
	FLinearColor BorderUnHovered;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationHighlight;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UToolTip> DefaultToolTip;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataDragAndDropPanel> ItemDataDragAndDropPanel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataDragDropOperation> ItemDataDragDropOperation;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void DraggedFromInventory(UItemDataDragDropOperation* Operation) const;
	void DraggedFromEquipment(UItemDataDragDropOperation* Operation, FItemData* InSlotData) const;
	void DraggedFromOtherInventory(UItemDataDragDropOperation* Operation) const;
	void OnEndDraggedFromOtherInventory(UItemDataDragDropOperation* Operation) const;

	bool ShowDropSlotAndGreenBorderColor(UItemDataDragDropOperation* Operation) const;
	bool ShowWrongSlotAndRedBorderColor(UItemDataDragDropOperation* Operation) const;
	bool ShowSwapSlotAndGreenBorderColor(UItemDataDragDropOperation* Operation) const;
	
	void OnClickedButtonItem();
	void OnDropKeyPressed() const;
	void ShowItemComparison() const;
	void HideItemComparison() const;

	void SetToolTipPositionAndAlignment() const;
	void RefreshTooltipGamepad();
	void RefreshToolTip();
	void DropOnTheGround() const;
	void EmptySlot();
	void SetItemQuantity() const;

	void OverwriteSlot(FItemData* ItemData);
	void SetButtonStyle(FItemData* ItemData) const;
	EWidgetType GetActiveWidgetType() const;
	bool AttemptSplitting(UItemDataDragDropOperation* Operation) const;
	bool IsUsingGamepad() const;

	void OpenInventoryWindow() const;
	void OpenVendorStorageWindow() const;
	void TryToUseAnItem() const;

private:
	UFUNCTION()
	void OnRightClick();
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();
	
	bool IsAnyPopUpActive() const;
	
	AMainPlayerController* CachedPlayerController;
	UWidgetManager* WidgetManager;
	UPlayerEquipment* CachedPlayerEquipment;
	UPlayerInventory* CachedPlayerInventory;
	UInventoryCore* CachedReceiverInventory;

	UPlayerInventoryWidget* CachedPlayerInventoryWidget;
	UToolTip* CachedToolTip;
	FIconButtonGameModule* IconButtonGameModule;

	FItemData* CurrentItemData;
	FItemData* EmptySlotData;
	FKey KeyInput;

	int32 SlotIndex;
	
	bool bIsInitialInputDelay;
	FTimerHandle DelayTimer;
	
	FVector2D ImageSize;
};
