#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "VendorSlot.generated.h"

class UItemDataDragAndDropPanel;
class UItemDataDragDropOperation;
class UWidgetManager;
class FIconButtonGameModule;
class UToolTip;
class UVendorInventory;
class UInventoryCore;

UCLASS()
class PROJECTEAST_API UVendorSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(UVendorInventory* InParentWidget, UInventoryCore* InActorInventory,
	                    UInventoryCore* InPlayerInventory, UWidgetManager* InWidgetManager, int32 InSlotIndex);
	void OverwriteSlot(FItemData* ItemData);
	void HighlightSlot();

protected:
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderSlot;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonItem;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextQuantity;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationHighlight;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor BorderHovered;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor BorderUnHovered;
	UPROPERTY(EditAnywhere)
	FVector2D DraggedImageSize;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UToolTip> ToolTipClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataDragAndDropPanel> ItemDataDragAndDropPanel;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataDragDropOperation> ItemDataDragDropOperation;

private:
	UPROPERTY()
	UInventoryCore* ActorInventory;
	UPROPERTY()
	UInventoryCore* PlayerInventory;
	UPROPERTY()
	UVendorInventory* ParentWidgetRef;
	UPROPERTY()
	UWidgetManager* WidgetManager;
	UPROPERTY()
	UToolTip* CachedToolTip;

	FIconButtonGameModule* IconButtonGameModule;

	FItemData* CurrentItemData = new FItemData();
	int32 SlotIndex;

	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply
	NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	                                  UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                              UDragDropOperation* InOperation) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void OnRightClick();
	UFUNCTION()
	void OnClickedButton();
	UFUNCTION()
	void OnHoveredButton();
	UFUNCTION()
	void OnUnhoveredButton();

	void ShowItemComparison() const;
	void HideItemComparison() const;

	FText GetQuantity() const;
	void SetButtonStyle(FItemData* ItemData) const;
	void EmptySlot();
	void RefreshToolTip();
	void RefreshGeometryCache();
	void SetToolTipPositionAndAlignment() const;
	bool IsUsingGamepad() const;
	bool IsAnyPopupActive() const;

	void DraggedFromInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const;
	void DraggedFromOtherInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const;
};
