#pragma once

#include "CoreMinimal.h"
#include "PlayerCraftingWindow.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingList.generated.h"

class UCraftingListSlot;
class UCraftingListSwitchButton;
class UCraftingListBox;

UCLASS()
class PROJECTEAST_API UCraftingList : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeList(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore);
	void ScrollToSlot(UUserWidget* TargetWidget) const;
	void SetCurrentlySelectedSlot(UCraftingListSlot* InSelectedSlot);
	void ClearCurrentlySelectedSlot() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCraftingListBox> CraftingListBoxClass;
	UPROPERTY(EditDefaultsOnly)
	TMap<ECraftingSubcategory, uint32> SubcategoriesPriority;

	UPROPERTY(meta=(BindWidget))
	UCraftingListSwitchButton* ButtonArmor;
	UPROPERTY(meta=(BindWidget))
	UCraftingListSwitchButton* ButtonWeapon;
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* MainVBoxArmor;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* MainVBoxWeapons;
	UPROPERTY(meta=(BindWidget))
	UScrollBox* ScrollBoxList;

private:
	UPROPERTY()
	UPlayerCrafting* CachedPlayerCrafting;
	UPROPERTY()
	UCraftingCore* CachedCraftingCore;

	UPROPERTY()
	UCraftingListSlot* SelectedSlot;
	
	TArray<ECraftingCategory> InUseCategories;
	TMap<ECraftingSubcategory, UCraftingListBox*> CraftingLists;
	ECraftingCategory CurrentListCategory;
	UFUNCTION()
	void SwitchList(ECraftingCategory Category);
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void ClearLists();
	void CreateCraftingList();
	void SelectFirstItemOnList();
	void SortCraftingListsBySubcategory();
	void RefreshListButtonsVisibility() const;
	void SwitchCraftingTab(bool IsToRight);
	void SetFocusToFirstSlot(bool IsConsiderLastFocus);
	void BuildCraftingList(TArray<FCraftingData*> CraftingList);
	void AssignCurrentlySelectedSlot(UCraftingListSlot* InSelectedSlot);
	void FillListsWithListBoxes(TArray<UCraftingListBox*> ArrayOfListBoxes);


	ECraftingCategory GetFirstValidListCategory();
	bool CanDisplayCraftable(FCraftingData* InCraftingData) const;
	FString GetListBoxTitleBySubcategory(ECraftingSubcategory Subcategory) const;
	UCraftingListBox* GetFirstListBoxFromCategory(ECraftingCategory Category);
};
