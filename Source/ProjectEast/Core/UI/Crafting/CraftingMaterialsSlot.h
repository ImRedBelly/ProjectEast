#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingMaterialsSlot.generated.h"


class FIconButtonGameModule;
class UToolTip;
class UCraftingMaterialsBar;

UCLASS()
class PROJECTEAST_API UCraftingMaterialsSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(FItemData* ItemData, int32 Quantity, int32 Index, UCraftingMaterialsBar* ParentMaterialsBar);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonItem;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderItem;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UImage* SlotBackground;
	UPROPERTY(meta=(BindWidget))
	UImage* SlotBackgroundGradient;
	UPROPERTY(meta=(BindWidget))
	UInvalidationBox* InvalidationBoxItem;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextAmountNeed;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextCurrentAmount;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextSlash;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UToolTip> ToolTipClass;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor GreenColor;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor RedColor;

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();

	void SetItemName() const;
	void SetItemImage() const;
	void SetCurrentItemAmount() const;
	void SetNeededItemAmount() const;
	void SetBackgroundColor() const;
	void SetBackgroundColorText() const;
	void SetTooltipPositionAndAlignment() const;
	void RefreshToolTipWidget();

	UPROPERTY()
	UCraftingMaterialsBar* ParentWidgetRef;
	UPROPERTY()
	UToolTip* ToolTipRef;
	
	FIconButtonGameModule* IconButtonGameModule;
	
	FItemData* CurrentItemData;

	int32 SlotIndex;
	int32 CurrentQuantity;
};
