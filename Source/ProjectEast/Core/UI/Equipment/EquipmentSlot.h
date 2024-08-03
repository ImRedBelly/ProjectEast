#pragma once

#include "CoreMinimal.h"
#include "EquipmentPanel.h"
#include "Components/Overlay.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "EquipmentSlot.generated.h"

UCLASS()
class PROJECTEAST_API UEquipmentSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	void EmptySlot() const;
	void OverwriteSlot(UEquipmentPanel* EquipmentPanel, FItemData* ItemData);
	
protected:
	UPROPERTY(EditAnywhere)
	EItemSlot EquipmentSLot; 
	UPROPERTY(meta=(BindWidget))
	UImage* BorderImage;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageUse;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonItem;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextLabel;
	UPROPERTY(meta=(BindWidget))
	UOverlay* OverlayLabel;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextQuantity;

	UPROPERTY(EditDefaultsOnly)
	FText LabelCharacterText;
	UPROPERTY(EditAnywhere)
	FSlateBrush EmptySlotStyle;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor BorderHovered;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor BorderUnHovered;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UToolTip> DefaultToolTip;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataDragAndDropPanel> ItemDataDragAndDropPanel;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataDragDropOperation> ItemDataDragDropOperation;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;
	

	void RefreshToolTip();
	void OnDropPressed();
	void SetItemQuantity() const;
	void DropOnTheGround();
	void SetToolTipPositionAndAlignment() const;

	void SetButtonStyle(FItemData* ItemData) const;

	ESlateVisibility GetLabelVisibility() const;
	FText GetLabelCharacter();
	EWidgetType GetActiveWidgetType() const;
	bool IsUsingGamepad() const;
	
private:
	UFUNCTION()
	void OnRightClick();
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();

	bool TryWeaponsSwapped(UItemDataDragDropOperation* Operation);
	bool ReturnWrongSlot(UItemDataDragDropOperation* Operation);
	
	UPlayerEquipment* CachedPlayerEquipment;
	UPlayerInventory* CachedPlayerInventory;
	UEquipmentPanel* CachedEquipmentPanel;
	UToolTip* CachedToolTip;

	FItemData* CurrentItemData;

};
