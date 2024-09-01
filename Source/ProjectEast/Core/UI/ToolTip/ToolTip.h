#pragma once

#include "CoreMinimal.h"
#include "ItemStatsSlot.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ProjectEast/Core/UI/Misc/ValueView.h"
#include "ProjectEast/Core/UI/Misc/WeightView.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ToolTip.generated.h"

class UUniformGridPanel;
class UHorizontalBox;
class UVerticalBox;
class UBorder;

DECLARE_DYNAMIC_DELEGATE(FOnShowComparison);
DECLARE_DYNAMIC_DELEGATE(FOnHideComparison);

UCLASS()
class PROJECTEAST_API UToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnShowComparison OnShowComparison;
	FOnHideComparison OnHideComparison;

	void InitializeToolTip(FItemData* ItemData, bool IsCompareItems);
	void UpdateItemStatsComparison(UToolTip* ComparedToolTip);
	void ShowComparisonToolTip();
	void HideComparisonToolTip();
	TMap<FString, UItemStatsSlot*> GetStatWidgets() { return StatWidgets; }

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderItemInfo;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBoxItemStats;
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* HorizontalBoxRequiredLevel;
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* HorizontalBoxDurability;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemType;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemRarity;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemDescription;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextRequiredLevel;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextRequiredLevelValue;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextDurability;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextDurabilityValue;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextAlreadyRead;

	UPROPERTY(meta=(BindWidget))
	UWeightView* WeightView;
	UPROPERTY(meta=(BindWidget))
	UValueView* ValueView;
	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* UniformGrid;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderEquipped;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ToolTipAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UToolTip> DefaultToolTip;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemStatsSlot> DefaultItemStatsSlot;

	virtual void NativeConstruct() override;

	void SetItemInfo();
	void SetItemName() const;
	void SetItemType() const;
	void SetItemRarity() const;
	void SetItemImage() const;
	void SetItemDescription() const;
	void SetItemRarityColor() const;
	void BuildItemStats();
	void SetItemWeight() const;
	void SetItemValue() const;
	void SetItemRequiredLevel() const;
	void SetItemRequiredLevelColor() const;
	void SetItemDurability() const;
	void SetItemDurabilityColor() const;
	void SetAlreadyReadVisibility() const;

private:
	UPlayerEquipment* PlayerEquipment;

	FItemData* CurrentItemData;
	TMap<FString, UItemStatsSlot*> StatWidgets;
	UToolTip* ComparisonToolTip;
	bool bIsCompareItems;
};
