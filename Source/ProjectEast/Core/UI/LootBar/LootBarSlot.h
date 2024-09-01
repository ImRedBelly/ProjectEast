#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "LootBarSlot.generated.h"

class AMainPlayerController;
class ULootBar;
class UButton;
class UImage;
class UTextBlock;

UCLASS()
class PROJECTEAST_API ULootBarSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(ULootBar* LootBar, FItemData* ItemData);

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* BackgroundButton;

	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;

	UPROPERTY(meta=(BindWidget))
	UImage* BorderImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemType;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemRarity;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemQuantity;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationBorder;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;

	void SetItemName() const;
	void SetItemType() const;
	void SetItemRarity() const;
	void SetItemRarityColor() const;
	void SetItemQuantity() const;
	void SetImageItem() const;

private:
	AMainPlayerController* CachedPlayerController;
	ULootBar* CachedLootBar;
	FItemData* CachedItemData;

	FIconButtonGameModule* IconButtonGameModule;
	
	UFUNCTION()
	void OnRightClick();

	UFUNCTION()
	void OnHovered();

	void OnMouseMoved();
	bool IsUsingGamepad() const;

	void SetActivateState();
	void ClearAnimation();
};
