#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/CraftingCore.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingBase.generated.h"

class UPlayerLeveling;
class UWeightView;
class UValueView;
class UCraftingQueue;
class UCraftingBar;
class UItemStatsSlot;
class UPlayerEquipment;
class UCraftingCore;
class UPlayerCrafting;
class UCraftingMaterialsBar;

UCLASS()
class PROJECTEAST_API UCraftingBase : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeCraftingBase(UPlayerCrafting* InPlayerCrafting, UCraftingCore* InCraftingCore);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageRequireLevelBackground;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextDescription;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemRarity;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemType;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextQueueAmount;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextMaxQueueAmount;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextRequireLevel;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderCraftingCounter;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonClearQueue;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBoxItemsStats;
	UPROPERTY(meta=(BindWidget))
	UItemStatsSlot* CraftingAttributesDamage;
	UPROPERTY(meta=(BindWidget))
	UItemStatsSlot* CraftingAttributesAttackSpeed;
	UPROPERTY(meta=(BindWidget))
	UCraftingBar* CraftingBar;
	UPROPERTY(meta=(BindWidget))
	UCraftingMaterialsBar* CraftingMaterialsBar;
	UPROPERTY(meta=(BindWidget))
	UCraftingQueue* CraftingQueue;
	UPROPERTY(meta=(BindWidget))
	UValueView* ValueView;
	UPROPERTY(meta=(BindWidget))
	UWeightView* WeightView;
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcherCraftingBox;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ItemInfoAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemStatsSlot> ItemStatsSlotClass;
	
	UPROPERTY(EditAnywhere)
	int32 RowLength;

private:
	UPROPERTY()
	UPlayerLeveling* PlayerLeveling;
	UPROPERTY()
	UPlayerEquipment* PlayerEquipment;
	UPROPERTY()
	UPlayerCrafting* PlayerCrafting;
	UPROPERTY()
	UCraftingCore* CraftingStation;
	UPROPERTY()
	UUserWidget* QueueFocusTransfer;

	FItemData* CurrentCraftableItem;
	TArray<FSingleDTItem> CurrentRequireMaterials;
	FCraftingData* StoredCraftingData;

	float SliderValue;
	float Time;
	int32 MaxAmount = 1;
	int32 CurrentAmount = 1;

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void OnClicked();
	UFUNCTION()
	void RefreshCraftingData();

	void SetCurrentCraftingData();
	void SetItemName() const;
	void SetItemType() const;
	void SetItemRarity() const;
	void SetItemImage() const;
	void SetItemDescription() const;
	void SetItemRarityColor() const;
	void BuildStatsSlots();
	void SetItemWeight() const;
	void SetItemValue() const;
	float GetComparisonValue(EItemSlot SlotType, FItemsStatRow Stat) const;
	void SetCurrentQueuedAmount() const;
	void SetMaxQueuedAmount() const;
	void SetRequiredLevelColor() const;
	void SetItemRequiredLevel() const;

	UWidget* Nav_CraftingBar(EUINavigation NavigationType);
	UWidget* Nav_MaterialsBar(EUINavigation NavigationType);
	UWidget* Nav_CraftingQueue(EUINavigation NavigationType);
	UWidget* Nav_ParentWidget(EUINavigation NavigationType);
};
