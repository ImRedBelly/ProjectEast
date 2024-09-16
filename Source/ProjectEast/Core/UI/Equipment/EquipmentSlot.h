#pragma once

#include "CoreMinimal.h"
#include "Components/Overlay.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "EquipmentSlot.generated.h"

class UPlayerInventory;
class UPlayerEquipment;
class AMainPlayerController;
class UItemDataDragDropOperation;
class UItemDataDragAndDropPanel;
class UEquipmentPanel;
class UWidgetManager;
class UToolTip;

UCLASS()
class PROJECTEAST_API UEquipmentSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	void EmptySlot();
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
	UPROPERTY(EditDefaultsOnly)
	FLinearColor GoodColor;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor BadColor;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UToolTip> DefaultToolTip;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemDataDragAndDropPanel> ItemDataDragAndDropPanel;
	UPROPERTY(EditDefaultsOnly)
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
	
	AMainPlayerController* PlayerController;
	UWidgetManager* WidgetManager;
	UPlayerEquipment* CachedPlayerEquipment;
	UPlayerInventory* CachedPlayerInventory;
	UEquipmentPanel* CachedEquipmentPanel;
	UToolTip* CachedToolTip;
	
	FIconButtonGameModule* IconButtonGameModule;

	UPROPERTY(EditDefaultsOnly)
	FItemData EmptyItemData;
	FItemData* CurrentItemData;

};
