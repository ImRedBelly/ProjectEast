#include "CraftingListBox.h"
#include "CraftingListSlot.h"
#include "CraftingListSpacer.h"

void UCraftingListBox::InitializeBox(UCraftingList* CraftingList, UPlayerCrafting* PlayerCrafting,
                                     ECraftingCategory Category, ECraftingSubcategory Subcategory,
                                     const FText& NameTitle)
{
	CachedCraftingList = CraftingList;
	CachedPlayerCrafting = PlayerCrafting;

	ListCategory = Category;
	ListSubcategory = Subcategory;
	CraftingListSpacer->SetListTitle(NameTitle);
}


void UCraftingListBox::AddNewSlot(FCraftingData* CraftingData)
{
	UCraftingListSlot* NewCraftingListSlot = CreateWidget<UCraftingListSlot>(this, CraftingListSlotClass);
	NewCraftingListSlot->InitializeSlot(CraftingData, CachedPlayerCrafting);
	VerticalBoxCraftableList->AddChildToVerticalBox(NewCraftingListSlot);
	NewCraftingListSlot->SetSlotIndex(this, VerticalBoxCraftableList->GetChildIndex(NewCraftingListSlot));
}


void UCraftingListBox::ClearSlotsInBox() const
{
	VerticalBoxCraftableList->ClearChildren();
}

void UCraftingListBox::OnSlotSelected(UCraftingListSlot* CraftingListSlot)
{
	if(IsValid(CachedCraftingList) && IsValid(CraftingListSlot))
	{
		CachedCraftingList->ClearCurrentlySelectedSlot();
		CachedCraftingList->SetCurrentlySelectedSlot(CraftingListSlot);
		FocusedIndex = CraftingListSlot->GetSlotIndex();
	}
}


void UCraftingListBox::OnSlotFocused(UCraftingListSlot* CraftingListSlot) const
{
	if(IsValid(CachedCraftingList))
		CachedCraftingList->ScrollToSlot(CraftingListSlot);
}

UCraftingListSlot* UCraftingListBox::GetCurrentlyFocusedSlot() const
{
	return GetSlotByIndex(FocusedIndex);
}

UCraftingListSlot* UCraftingListBox::GetSlotByIndex(int32 Index) const
{
	if (UWidget* ChildWidget = VerticalBoxCraftableList->GetChildAt(Index))
		return Cast<UCraftingListSlot>(ChildWidget);

	return nullptr;
}

FReply UCraftingListBox::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UCraftingListSlot* CraftingListSlot = GetSlotByIndex(0);
	if (IsValid(CraftingListSlot) && VerticalBoxCraftableList->IsVisible())
	{
		CraftingListSlot->SetKeyboardFocus();
		return FReply::Handled();
	}

	CraftingListSpacer->SetKeyboardFocus();
	return FReply::Handled();
}
