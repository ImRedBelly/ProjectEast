#include "CraftingMaterialsBar.h"

#include "CraftingMaterialsSlot.h"
#include "Components/UniformGridSlot.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingMaterialsBar::InitializeWidget(UPlayerCrafting* InPlayerCrafting)
{
	PlayerCrafting = InPlayerCrafting;

	if (IsValid(PlayerCrafting))
	{
		CraftingStation = PlayerCrafting->GetCurrentStationComponent();
		if (IsValid(CraftingStation))
		{
			RefreshCraftingData();
			BindEventDispatchers();
		}
	}
}

void UCraftingMaterialsBar::SetCurrentFocusedSlot(int32 Index)
{
	CurrentlyFocusedSlot = Index;
}

void UCraftingMaterialsBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsDesignTime())
		BuildSampleMaterialsSlots();
}

void UCraftingMaterialsBar::NativeDestruct()
{
	Super::NativeDestruct();
	UnbindEventDispatchers();
}

FReply UCraftingMaterialsBar::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToSlot(CurrentlyFocusedSlot);
	return FReply::Handled();
}

void UCraftingMaterialsBar::OnNewItemSelected(FCraftingData& CraftingData)
{
	CurrentlyFocusedSlot = 0;
}

void UCraftingMaterialsBar::SetCurrentCraftingData()
{
	auto Data = PlayerCrafting->GetCurrentCraftableData();
	RequiredMaterials = Data.Get<1>();
	CurrentCraftableItem = Data.Get<0>();
}

void UCraftingMaterialsBar::RefreshCraftingData()
{
	SetCurrentCraftingData();
	BuildMaterialsSlots();
	if (HasKeyboardFocus() || HasUserFocusedDescendants(GetOwningPlayer()))
	{
		SetFocusToSlot(CurrentlyFocusedSlot);
	}
}

void UCraftingMaterialsBar::BindEventDispatchers()
{
	PlayerCrafting->OnRefreshed.AddDynamic(this, &UCraftingMaterialsBar::RefreshCraftingData);
	PlayerCrafting->OnNewItemSelected.AddDynamic(this, &UCraftingMaterialsBar::OnNewItemSelected);
	CraftingStation->OnRefreshed.AddDynamic(this, &UCraftingMaterialsBar::RefreshCraftingData);
}

void UCraftingMaterialsBar::UnbindEventDispatchers()
{
	if (IsValid(PlayerCrafting))
	{
		PlayerCrafting->OnRefreshed.RemoveDynamic(this, &UCraftingMaterialsBar::RefreshCraftingData);
		PlayerCrafting->OnNewItemSelected.RemoveDynamic(this, &UCraftingMaterialsBar::OnNewItemSelected);
	}
	if (IsValid(CraftingStation))
		CraftingStation->OnRefreshed.RemoveDynamic(this, &UCraftingMaterialsBar::RefreshCraftingData);
}

void UCraftingMaterialsBar::BuildMaterialsSlots()
{
	int32 Row = 0;
	int32 Column = 0;;
	UniformGridMaterials->ClearChildren();
	for (int i = 0; i < RequiredMaterials.Num(); ++i)
	{
		FItemData* ItemData = RequiredMaterials[i].TableAndRow.DataTable->FindRow<FItemData>(
			RequiredMaterials[i].TableAndRow.RowName, TEXT(""));
		if (InventoryUtility::IsItemClassValid(ItemData))
		{
			auto NewItemData = InventoryUtility::CopyItemData(ItemData);
			NewItemData->Quantity = RequiredMaterials[i].Quantity;

			int32 Quantity = PlayerCrafting->FindItemQuantity(NewItemData, GetOwningPlayer());
			UCraftingMaterialsSlot* MaterialsSlot = CreateWidget<UCraftingMaterialsSlot>(this, CraftingMaterialsSlotClass);
			MaterialsSlot->InitializeSlot(NewItemData, Quantity, i, this);
			
			UUniformGridSlot* Element = UniformGridMaterials->AddChildToUniformGrid(MaterialsSlot, Row, Column);
			Element->SetHorizontalAlignment(HAlign_Fill);
			Element->SetVerticalAlignment(VAlign_Fill);
			
			Column++;
			if (Column >= RowLength)
			{
				Column = 0;
				Row++;
			}
		}
	}
}

void UCraftingMaterialsBar::BuildSampleMaterialsSlots()
{
	if (!IsValid(CraftingMaterialsSlotClass))
		return;

	int32 Row = 0;
	int32 Column = 0;
	UniformGridMaterials->ClearChildren();

	for (int i = 0; i < 5; ++i)
	{
		UCraftingMaterialsSlot* MaterialsSlot = CreateWidget<UCraftingMaterialsSlot>(this, CraftingMaterialsSlotClass);

		UUniformGridSlot* Element = UniformGridMaterials->AddChildToUniformGrid(MaterialsSlot, Row, Column);
		Element->SetHorizontalAlignment(HAlign_Fill);
		Element->SetVerticalAlignment(VAlign_Fill);

		Column++;
		if (Column >= RowLength)
		{
			Column = 0;
			Row++;
		}
	}
}

void UCraftingMaterialsBar::SetFocusToSlot(int32 Index)
{
	auto SelectedChild = UniformGridMaterials->GetChildAt(Index);
	if (IsValid(SelectedChild))
	{
		SelectedChild->SetKeyboardFocus();
	}
	else
	{
		auto FirstChild = UniformGridMaterials->GetChildAt(Index);
		if (IsValid(FirstChild))
			FirstChild->SetKeyboardFocus();
	}
}
