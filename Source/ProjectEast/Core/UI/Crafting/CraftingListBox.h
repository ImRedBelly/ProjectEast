#pragma once

#include "CoreMinimal.h"
#include "CraftingList.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingListBox.generated.h"

class UCraftingListSpacer;
class UCraftingListSlot;
class FIconButtonGameModule;

UCLASS()
class PROJECTEAST_API UCraftingListBox : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeBox(UCraftingList* Craftings, UPlayerCrafting* PlayerCrafting, ECraftingCategory Category,
	                   ECraftingSubcategory Subcategory, const FString& NameTitle);


	void AddNewSlot(FCraftingData* CraftingData);
	void ClearSlotsInBox() const;
	void OnSlotSelected(UCraftingListSlot* CraftingListSlot);
	void OnSlotFocused(UCraftingListSlot* CraftingListSlot) const;
	UCraftingListSlot* GetCurrentlyFocusedSlot() const;
	UCraftingListSlot* GetSlotByIndex(int32 Index) const;

	ECraftingCategory GetListCategory() const { return ListCategory; }
	ECraftingSubcategory GetListSubcategory() const { return ListSubcategory; }

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCraftingListSlot> CraftingListSlotClass;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBoxCraftableList;
	UPROPERTY(meta=(BindWidget))
	UCraftingListSpacer* CraftingListSpacer;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

private:
	UPROPERTY()
	UCraftingList* CachedCraftingList;
	
	UPROPERTY()
	UPlayerCrafting* CachedPlayerCrafting;

	ECraftingCategory ListCategory;
	ECraftingSubcategory ListSubcategory;
	int32 FocusedIndex;
};
