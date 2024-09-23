#include "LootBar.h"
#include "LootBarSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void ULootBar::SetCurrentlyFocusedSlot(ULootBarSlot* LootBarSlot)
{
	if (IsValid(LootBarSlot))
		FocusedIndex = VerticalBox->GetChildIndex(LootBarSlot);
}

void ULootBar::ScrollWidget(ULootBarSlot* LootBarSlot) const
{
	ScrollBox->ScrollWidgetIntoView(LootBarSlot, true, EDescendantScrollDestination::Center, 0.0f);
}

void ULootBar::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonTake->OnClicked.AddUniqueDynamic(this, &ULootBar::TakeItem);
	ButtonTakeAll->OnClicked.AddUniqueDynamic(this, &ULootBar::TakeAllItems);
	ButtonClose->OnClicked.AddUniqueDynamic(this, &ULootBar::Close);
}

void ULootBar::BindEventDispatchers()
{
	PlayerInventory->OnTakeItem.AddDynamic(this, &ULootBar::TakeItem);
	PlayerInventory->OnTakeAllItems.AddDynamic(this, &ULootBar::TakeAllItems);
	OwnerInventory->OnRefreshInventory.AddDynamic(this, &ULootBar::RefreshLootBar);
}

void ULootBar::UnbindEventDispatchers()
{
	if (IsValid(PlayerInventory))
	{
		PlayerInventory->OnTakeItem.RemoveDynamic(this, &ULootBar::TakeItem);
		PlayerInventory->OnTakeAllItems.RemoveDynamic(this, &ULootBar::TakeAllItems);
	}
	if (IsValid(OwnerInventory))
		OwnerInventory->OnRefreshInventory.RemoveDynamic(this, &ULootBar::RefreshLootBar);
}

void ULootBar::PlayAnimationConstruct()
{
	PlayAnimation(AnimationConstruct, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void ULootBar::CreateLootBar()
{
	VerticalBox->ClearChildren();


	auto DataItems = OwnerInventory->GetInventoryAndSize(EInventoryPanels::P1).Get<0>();

	for (FItemData* ItemData : DataItems)
	{
		if (InventoryUtility::IsItemClassValid(ItemData))
		{
			ULootBarSlot* NewBarSlot = CreateWidget<ULootBarSlot>(GWorld->GetGameInstance(), DefaultLootBarSlot);
			NewBarSlot->InitializeSlot(this, ItemData);
			VerticalBox->AddChildToVerticalBox(NewBarSlot);
		}
	}
}

void ULootBar::ActivateLootBar()
{
	PlayerInventory = InventoryUtility::GetPlayerInventory(GetOwningPlayer());
	SetOwnerInventory();
	if (IsValid(OwnerInventory))
	{
		BindEventDispatchers();
		PlayAnimationConstruct();
		CreateLootBar();
	}
}

void ULootBar::DeactivateLootBar()
{
	UnbindEventDispatchers();
	FocusedIndex = 0;
	ScrollBox->ScrollToStart();
}

void ULootBar::SetOwnerInventory()
{
	auto DataInventory = InventoryUtility::GetInventoryFromInteractable(
		InventoryUtility::GetCurrentInteractableObject(GetOwningPlayer()));

	if (DataInventory.Get<0>())
		OwnerInventory = DataInventory.Get<1>();
}

void ULootBar::SetFocusToSlot(int32 Index)
{
	if (IsValid(VerticalBox->GetChildAt(Index)))
		SetKeyboardFocus();
	else
	{
		Index--;
		if (IsValid(VerticalBox->GetChildAt(Index)))
			SetKeyboardFocus();
	}
}

bool ULootBar::IsValidateOwnerInventory() const
{
	auto DataInventory = OwnerInventory->GetInventoryAndSize(EInventoryPanels::P1);
	return DataInventory.Get<0>().IsValidIndex(0);
}

void ULootBar::TakeItem()
{
	auto Inventory = OwnerInventory->GetInventoryAndSize(EInventoryPanels::P1).Get<0>();

	if(FocusedIndex >= Inventory.Num())
		FocusedIndex = 0;
	auto ItemData = Inventory[FocusedIndex];
	
	if (InventoryUtility::IsItemClassValid(ItemData))
		PlayerInventory->ServerTakeItem(ItemData, OwnerInventory, GetOwningPlayer());
	if (IsValid(OwnerInventory))
	{
		if (OwnerInventory->GetInventoryAndSize(EInventoryPanels::P1).Get<0>().IsValidIndex(0))
		{
			SetFocusToSlot(FocusedIndex);
		}
	}
}

void ULootBar::TakeAllItems()
{
	PlayerInventory->ServerTakeAllItems(OwnerInventory, GetOwningPlayer());
	SetFocusToSlot(0);
}

void ULootBar::Close()
{
	PlayerInventory->InputCloseWidget();
}


void ULootBar::RefreshLootBar(EInventoryPanels Panel)
{
	if (IsValidateOwnerInventory())
	{
		CreateLootBar();
		SetFocusToSlot(FocusedIndex);
	}
}
