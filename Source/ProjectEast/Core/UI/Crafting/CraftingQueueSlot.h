#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingQueueSlot.generated.h"

class FIconButtonGameModule;
class UCraftingQueue;
class UCraftingCore;

UCLASS()
class PROJECTEAST_API UCraftingQueueSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeSlot(UCraftingCore* InCraftingStation, FCraftingData* InCraftingData, UCraftingQueue* ParentWidget, int32 Index);
protected:

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonMain;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonCancel;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderMain;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderCancel;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextCancel;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBarCrafting;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HoveredAnimation;

private:
	UPROPERTY()
	UCraftingCore* CraftingStation;
	UPROPERTY()
	UCraftingQueue* ParentWidgetRef;
	FIconButtonGameModule* IconButtonGameModule;

	FCraftingData* CraftingData;
	int32 AmountToCraft;
	int32 SlotIndex;
	bool bIsFocused;

	virtual void NativeDestruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void OnClicked();
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();

	void SelectSlot();
	void StopCraftingItem() const;
	void SetCraftingPercentage() const;
	void SetTextColor() const;
	FText GetCurrentAmount() const;
	FText GetMaxAmount() const;
};
