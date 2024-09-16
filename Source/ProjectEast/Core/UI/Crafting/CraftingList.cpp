#include "CraftingList.h"
#include "CraftingListBox.h"
#include "CraftingListSlot.h"
#include "CraftingListSwitchButton.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingList::InitializeList(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore)
{
	CachedPlayerCrafting = InPlayerCraftingCore;
	CachedCraftingCore = InCraftingCore;
	CreateCraftingList();
}

void UCraftingList::ScrollToSlot(UUserWidget* TargetWidget) const
{
	if (IsValid(TargetWidget))
	{
		ScrollBoxList->ScrollWidgetIntoView(TargetWidget, true, EDescendantScrollDestination::Center, 0.0f);
	}
}

void UCraftingList::SetCurrentlySelectedSlot(UCraftingListSlot* InSelectedSlot)
{
	if (IsValid(InSelectedSlot))
	{
		SelectedSlot = InSelectedSlot;
	}
}

void UCraftingList::ClearCurrentlySelectedSlot() const
{
	if (IsValid(SelectedSlot))
	{
		SelectedSlot->UnselectSlot();
	}
}

void UCraftingList::SwitchList(ECraftingCategory Category)
{
	if (CurrentListCategory != Category && InUseCategories.Contains(Category))
	{
		CurrentListCategory = Category;
		InventoryUtility::PlaySoundOnTabSwitched();

		if (CurrentListCategory == ECraftingCategory::Armor)
			WidgetSwitcher->SetActiveWidget(MainVBoxArmor);
		if (CurrentListCategory == ECraftingCategory::Weapons)
			WidgetSwitcher->SetActiveWidget(MainVBoxWeapons);

		ButtonArmor->RefreshListButton(CurrentListCategory);
		ButtonWeapon->RefreshListButton(CurrentListCategory);
		SetFocusToFirstSlot(false);
	}
}

void UCraftingList::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonArmor->OnClickedButton.AddDynamic(this, &UCraftingList::SwitchList);
	ButtonWeapon->OnClickedButton.AddDynamic(this, &UCraftingList::SwitchList);
}

void UCraftingList::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonArmor->OnClickedButton.RemoveDynamic(this, &UCraftingList::SwitchList);
	ButtonWeapon->OnClickedButton.RemoveDynamic(this, &UCraftingList::SwitchList);
}

FReply UCraftingList::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToFirstSlot(true);
	return FReply::Handled();
}

void UCraftingList::CreateCraftingList()
{
	if (IsValid(CachedCraftingCore))
	{
		BuildCraftingList(CachedCraftingCore->GetCraftingListArray());
	}
}

void UCraftingList::SelectFirstItemOnList()
{
}

void UCraftingList::RefreshListButtonsVisibility() const
{
	ButtonArmor->SetVisibility(InUseCategories.Contains(ButtonArmor->GetCraftingCategory())
		                           ? ESlateVisibility::Visible
		                           : ESlateVisibility::Collapsed);
	ButtonWeapon->SetVisibility(InUseCategories.Contains(ButtonWeapon->GetCraftingCategory())
		                            ? ESlateVisibility::Visible
		                            : ESlateVisibility::Collapsed);
}

void UCraftingList::SwitchCraftingTab(bool IsToRight)
{
}

void UCraftingList::SortCraftingListsBySubcategory()
{
	TArray<UCraftingListBox*> ArrayOfListBoxes;

	for (auto KV : CraftingLists)
	{
		UCraftingListBox* CurrentBox = CraftingLists[KV.Key];
		if (SubcategoriesPriority.Contains(KV.Key))
		{
			uint32 ValuePriority = SubcategoriesPriority[KV.Key];

			bool IsElementInserted = false;
			for (int i = 0; i < ArrayOfListBoxes.Num(); ++i)
			{
				if (!SubcategoriesPriority.Contains(ArrayOfListBoxes[i]->GetListSubcategory()) ||
					SubcategoriesPriority[ArrayOfListBoxes[i]->GetListSubcategory()] <= ValuePriority)
					continue;
				IsElementInserted = true;
				ArrayOfListBoxes.Insert(CurrentBox, ValuePriority);
			}

			if (!IsElementInserted)
				ArrayOfListBoxes.Add(CurrentBox);
		}
		else
			ArrayOfListBoxes.Add(CurrentBox);
	}
	MainVBoxArmor->ClearChildren();
	MainVBoxWeapons->ClearChildren();

	FillListsWithListBoxes(ArrayOfListBoxes);
}

void UCraftingList::SetFocusToFirstSlot(bool IsConsiderLastFocus)
{
	if (IsConsiderLastFocus && IsValid(SelectedSlot))
		SelectedSlot->SetKeyboardFocus();
	else
	{
		if (UCraftingListBox* CraftingListBox = GetFirstListBoxFromCategory(CurrentListCategory))
			CraftingListBox->SetKeyboardFocus();
	}
}

void UCraftingList::BuildCraftingList(TArray<FCraftingData*> CraftingList)
{
	CraftingLists.Empty();
	InUseCategories.Empty();
	MainVBoxArmor->ClearChildren();
	MainVBoxWeapons->ClearChildren();

	for (auto CraftingData : CraftingList)
	{
		if (CanDisplayCraftable(CraftingData))
		{
			InUseCategories.AddUnique(CraftingData->Category);

			if (CraftingLists.Contains(CraftingData->Subcategory))
			{
				CraftingLists[CraftingData->Subcategory]->AddNewSlot(CraftingData);
			}
			else
			{
				UCraftingListBox* NewCraftingListBox = CreateWidget<UCraftingListBox>(this, CraftingListBoxClass);
				NewCraftingListBox->InitializeBox(this, CachedPlayerCrafting, CraftingData->Category,
				                                  CraftingData->Subcategory,
				                                  GetListBoxTitleBySubcategory(CraftingData->Subcategory));

				CraftingLists.Add(CraftingData->Subcategory, NewCraftingListBox);
				NewCraftingListBox->AddNewSlot(CraftingData);

				if (CraftingData->Category == ECraftingCategory::Armor)
					MainVBoxArmor->AddChildToVerticalBox(NewCraftingListBox);
				if (CraftingData->Category == ECraftingCategory::Weapons)
					MainVBoxWeapons->AddChildToVerticalBox(NewCraftingListBox);
			}
		}
	}
	RefreshListButtonsVisibility();
	SortCraftingListsBySubcategory();
}

void UCraftingList::AssignCurrentlySelectedSlot(UCraftingListSlot* InSelectedSlot)
{
}

void UCraftingList::FillListsWithListBoxes(TArray<UCraftingListBox*> ArrayOfListBoxes)
{
	for (auto ArrayOfListBox : ArrayOfListBoxes)
	{
		if (IsValid(ArrayOfListBox))
		{
			if (ArrayOfListBox->GetListCategory() == ECraftingCategory::Armor)
				MainVBoxArmor->AddChildToVerticalBox(ArrayOfListBox);
			if (ArrayOfListBox->GetListCategory() == ECraftingCategory::Weapons)
				MainVBoxWeapons->AddChildToVerticalBox(ArrayOfListBox);
		}
	}
}

ECraftingCategory UCraftingList::GetFirstValidListCategory()
{
	for (auto Child : WidgetSwitcher->GetAllChildren())
	{
		if (auto VerticalBox = Cast<UVerticalBox>(Child))
		{
			if (VerticalBox->HasAnyChildren())
			{
				for (auto CraftingCategory : InUseCategories)
				{
					if (CraftingCategory == ECraftingCategory::Armor && VerticalBox == MainVBoxArmor)
						return CraftingCategory;
					if (CraftingCategory == ECraftingCategory::Weapons && VerticalBox == MainVBoxWeapons)
						return CraftingCategory;
				}
			}
		}
	}
	return ECraftingCategory::None;
}

bool UCraftingList::CanDisplayCraftable(FCraftingData* InCraftingData) const
{
	auto CraftableData = InventoryUtility::GetCraftableData(InCraftingData);

	if (InventoryUtility::IsItemClassValid(CraftableData.Get<0>()[0]))
	{
		if (CachedCraftingCore->GetIsShowLockedItems() ||
			!CachedPlayerCrafting->IsCraftingRecipeLocked(InCraftingData))
			return true;
	}
	return false;
}

FString UCraftingList::GetListBoxTitleBySubcategory(ECraftingSubcategory Subcategory) const
{
	return UEnum::GetValueAsString(Subcategory);
}

UCraftingListBox* UCraftingList::GetFirstListBoxFromCategory(ECraftingCategory Category)
{
	if (Category == ECraftingCategory::Armor)
		return Cast<UCraftingListBox>(MainVBoxArmor->GetAllChildren()[0]);
	if (Category == ECraftingCategory::Weapons)
		return Cast<UCraftingListBox>(MainVBoxWeapons->GetAllChildren()[0]);
	return nullptr;
}

void UCraftingList::ClearLists()
{
}
