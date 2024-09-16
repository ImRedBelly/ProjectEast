#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingListSlot.generated.h"

class UWidgetManager;
class UToolTip;
struct FItemData;
class FIconButtonGameModule;
class UCraftingListBox;
class UPlayerCrafting;

UCLASS()
class PROJECTEAST_API UCraftingListSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(FCraftingData* InCraftingData, UPlayerCrafting* InPlayerCrafting);
	void SetSlotIndex(UCraftingListBox* InCraftingListBox, int32 Index);

	void SelectSlot();
	void UnselectSlot();
	int32 GetSlotIndex() const { return CurrentIndex; }

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonItem;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageSlotBackground;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageRarityLabel;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageLock;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemType;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemRarity;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FocusAnim;

	UPROPERTY(EditAnywhere)
	FButtonStyle SelectedStyle;

	UPROPERTY(EditAnywhere)
	FButtonStyle UnselectedStyle;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UToolTip> ToolTipClass;

private:
	UPROPERTY()
	UPlayerCrafting* PlayerCrafting;
	UPROPERTY()
	UCraftingListBox* CraftingListBox;

	UToolTip* CachedToolTip;

	FItemData* ItemData;
	FCraftingData* CraftingData;

	UWidgetManager* WidgetManager;
	FIconButtonGameModule* IconButtonGameModule;

	int32 CurrentIndex;
	bool bIsSelected;


	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void AttemptSelectingSlot();
	void ShowItemComparison() const;
	void HideItemComparison() const;
	void SetToolTipPositionAndAlignment();
	void SetItemData();
	void RefreshGeometryCache();
	void SetItemImage() const;
	void SetItemName() const;
	void SetItemType() const;
	void SetItemRarity() const;
	void SetItemRarityColor() const;
	void SetTextColor() const;

	UFUNCTION()
	void OnClicked();
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();
};
