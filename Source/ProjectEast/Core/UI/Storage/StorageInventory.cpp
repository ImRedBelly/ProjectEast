#include "StorageInventory.h"
#include "StorageSlot.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UStorageInventory::NativeConstruct()
{
	Super::NativeConstruct();
	PlayAnimation(AnimationConstruct, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);

	auto PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	CachedPlayerInventory = PlayerController->GetPlayerInventory();
	WidgetManager = PlayerController->GetWidgetManager();
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);

	SetOwnerInventory();
	if (IsValid(CachedOwnerInventory))
	{
		BindEventDispatchers();
		CreateInventoryP1();
	}
}

void UStorageInventory::NativeDestruct()
{
	Super::NativeDestruct();
	UnbindEventDispatchers();
	if (CachedOwnerInventory->IsRefreshOnClosingWidget())
		CachedPlayerInventory->ServerSortInventory(CachedOwnerInventory, ESortMethod::Quicksort, EInventoryPanels::P1,
		                                           false);
}

void UStorageInventory::NativePreConstruct()
{
	Super::NativePreConstruct();
	DisplaySampleSlots(SampleSlotsToDisplay);
}

FReply UStorageInventory::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToSlot(FocusedSlot);
	return FReply::Handled();
}

void UStorageInventory::ResetSlotFocus()
{
	if (WidgetManager->GetCurrentlyFocusedWidget() == EWidgetType::Storage)
		SetFocusToSlot(FocusedSlot);
}

void UStorageInventory::CreateInventoryP1()
{
	auto DataInventory = CachedOwnerInventory->GetInventoryAndSize(EInventoryPanels::P1);
	BuildInventorySlots(DataInventory.Get<0>(), DataInventory.Get<1>(), InventoryUG1);
}

void UStorageInventory::SetOwnerInventory()
{
	CachedOwnerInventory = InventoryUtility::GetInventoryFromInteractable(
		InventoryUtility::GetCurrentInteractableObject(GetOwningPlayer())).Get<1>();
}

void UStorageInventory::BindEventDispatchers()
{
	CachedOwnerInventory->OnRefreshInventory.AddDynamic(this, &UStorageInventory::RefreshInventory);
	CachedOwnerInventory->OnHighlightInventorySlot.AddDynamic(this, &UStorageInventory::HighlightSlot);
	CachedOwnerInventory->OnSwitchedActivePanel.AddDynamic(this, &UStorageInventory::SwitchInventoryPanel);
}

void UStorageInventory::UnbindEventDispatchers()
{
	if (IsValid(CachedOwnerInventory))
	{
		CachedOwnerInventory->OnRefreshInventory.RemoveDynamic(this, &UStorageInventory::RefreshInventory);
		CachedOwnerInventory->OnHighlightInventorySlot.RemoveDynamic(this, &UStorageInventory::HighlightSlot);
		CachedOwnerInventory->OnSwitchedActivePanel.RemoveDynamic(this, &UStorageInventory::SwitchInventoryPanel);
	}
}

void UStorageInventory::DisplaySampleSlots(int32 IndexSlot)
{
	if (!IsValid(DefaultStorageSlot))
		return;

	uint32 CurrentRow = 0;
	uint32 CurrentColumn = 0;
	auto CurrentPanel = GetUniformGridFromPanel(EInventoryPanels::P1);
	CurrentPanel->ClearChildren();
	for (int i = 0; i < IndexSlot; ++i)
	{
		UStorageSlot* NewUStorageSlot = CreateWidget<UStorageSlot>(this, DefaultStorageSlot);
		UUniformGridSlot* CurrentSlot = CurrentPanel->AddChildToUniformGrid(
			NewUStorageSlot, CurrentRow, CurrentColumn);
		CurrentSlot->SetHorizontalAlignment(HAlign_Fill);
		CurrentSlot->SetVerticalAlignment(VAlign_Fill);
		CurrentColumn++;
		if (CurrentColumn >= RowLength)
		{
			CurrentColumn = 0;
			CurrentRow++;
		}
	}
}

void UStorageInventory::SetFocusToSlot(uint32 SlotIndex)
{
	if (UWidget* Widget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(SlotIndex))
	{
		FocusedSlot = SlotIndex;
		Widget->SetKeyboardFocus();
	}
}

void UStorageInventory::SwitchInventoryPanel(EInventoryPanels NewPanel)
{
	if (GetActivePanel() != NewPanel)
	{
		InventoryUtility::PlaySoundOnTabSwitched();
		PanelSwitcher->SetActiveWidget(GetUniformGridFromPanel(NewPanel));
		CachedOwnerInventory->SwitchedActivePanel(NewPanel);
	}
}

void UStorageInventory::ScrollToSlot(UStorageSlot* StorageSlot) const
{
	ScrollBox->ScrollWidgetIntoView(StorageSlot, true, EDescendantScrollDestination::Center, 0.0f);
}

void UStorageInventory::AssignCurrentlyFocusedSlot(UStorageSlot* StorageSlot)
{
	if (IsValid(StorageSlot))
		FocusedSlot = GetUniformGridFromPanel(GetActivePanel())->GetChildIndex(StorageSlot);
}

void UStorageInventory::BuildInventorySlots(TArray<FItemData*> ItemData, int32 Size, UUniformGridPanel* GridPanel)
{
	uint32 CurrentRow = 0;
	uint32 CurrentColumn = 0;

	GridPanel->ClearChildren();

	for (int i = 0; i < Size; ++i)
	{
		FItemData* CurrentItemData = new FItemData();
		if (ItemData.IsValidIndex(i))
			CurrentItemData = ItemData[i];
		else
			CurrentItemData->Index = i;

		UStorageSlot* StorageSlot = CreateWidget<UStorageSlot>(GetOwningPlayer(), DefaultStorageSlot);
		StorageSlot->InitializeSlot(CurrentItemData, this, CachedOwnerInventory, CachedPlayerInventory, IconButtonGameModule, i);

		UUniformGridSlot* CurrentSlot = GridPanel->AddChildToUniformGrid(StorageSlot, CurrentRow, CurrentColumn);
		CurrentSlot->SetHorizontalAlignment(HAlign_Fill);
		CurrentSlot->SetVerticalAlignment(VAlign_Fill);
		CurrentColumn++;
		if (CurrentColumn >= RowLength)
		{
			CurrentColumn = 0;
			CurrentRow++;
		}
	}
}

UUniformGridPanel* UStorageInventory::GetUniformGridFromPanel(EInventoryPanels Panel) const
{
	switch (Panel)
	{
	case EInventoryPanels::P1:
		return InventoryUG1;
	default:
		return nullptr;
	}
}

void UStorageInventory::HighlightSlot(uint32 SlotIndex)
{
	if (UStorageSlot* CurrentSlot = Cast<UStorageSlot>(
		GetUniformGridFromPanel(GetActivePanel())->GetChildAt(SlotIndex)))
		CurrentSlot->HighlightSlot();
}

void UStorageInventory::RefreshInventory(EInventoryPanels Panel)
{
	switch (Panel)
	{
	case EInventoryPanels::P1:
	case EInventoryPanels::P2:
	case EInventoryPanels::P3:
	case EInventoryPanels::P4:
		{
			CreateInventoryP1();
			ResetSlotFocus();
		}
		break;
	default: ;
	}
}
