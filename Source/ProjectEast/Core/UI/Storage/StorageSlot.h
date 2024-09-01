#pragma once

#include "CoreMinimal.h"
#include "StorageInventory.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/UI/ToolTip/ToolTip.h"
#include "StorageSlot.generated.h"

class UWidgetManager;
class AMainPlayerController;
class UButton;

UCLASS()
class PROJECTEAST_API UStorageSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(FItemData* ItemData, UStorageInventory* ParentWidget, UInventoryCore* OwnerInv,
		UPlayerInventory* PlayerInv, int IndexSlot);
	
	void HighlightSlot();

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonItem;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderObject;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextQuantity;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationHighlight;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor BorderHovered;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor BorderUnHovered;
	UPROPERTY(EditDefaultsOnly)
	FVector2D DraggedImageSize = FVector2D(100,100);
	
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
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void ShowItemComparison() const;
	void HideItemComparison() const;

	UFUNCTION()
	void OnItemClick();
	UFUNCTION()
	void OnItemHovered();
	UFUNCTION()
	void OnItemUnhovered();

private:
	UInventoryCore* ActorInventory;
	UInventoryCore* PlayerInventory;

	AMainPlayerController* CachedPlayerController;
	UWidgetManager* WidgetManager;
	UStorageInventory* CachedParentWidget;
	UToolTip* CachedToolTip;

	FItemData* CurrentItemData;
	uint32 SlotIndex;
	
	FIconButtonGameModule* IconButtonGameModule;

	FText GetQuantity() const;
	void SetButtonStyle(FItemData* ItemData) const;
	void EmptySlot();
	void OverwriteSlot(FItemData* ItemData);
	void RefreshToolTip();
	void RefreshGeometryCache();
	bool IsUsingGamepad() const;
	void SetToolTipPositionAndAlignment() const;
	bool IsAnyPopupActive() const;

	void DraggedFromInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const;
	void DraggedFromOtherInventory(UItemDataDragDropOperation* Operation, FItemData* ItemData) const;
};
