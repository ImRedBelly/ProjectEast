#include "VendorInventory.h"

#include "VendorSlot.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/KismetTextLibrary.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UVendorInventory::InitializeVendorInventory(UPlayerInventory* InPlayerInventory, UWidgetManager* InWidgetManager)
{
	PlayAnimation(ConstructAnimation);
	PlayerInventory = InPlayerInventory;
	WidgetManager = InWidgetManager;
	SetOwnerInventory();
	BindEventDispatchers();
	CreateInventoryP1();
	SetGoldValue();
}

void UVendorInventory::NativePreConstruct()
{
	Super::NativePreConstruct();
	DisplaySampleSlots(SampleSlotsToDisplay);
}

void UVendorInventory::NativeDestruct()
{
	UnbindEventDispatchers();
	if (OwnerInventory->IsRefreshOnClosingWidget())
		PlayerInventory->ServerSortInventory(OwnerInventory, ESortMethod::Quicksort, EInventoryPanels::P1, false);

	Super::NativeDestruct();
}

FReply UVendorInventory::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToSlot(FocusedSlot);
	return FReply::Handled();
}

void UVendorInventory::BindEventDispatchers()
{
	if (IsValid(OwnerInventory))
	{
		OwnerInventory->OnRefreshInventory.AddDynamic(this, &UVendorInventory::RefreshInventory);
		OwnerInventory->OnHighlightInventorySlot.AddDynamic(this, &UVendorInventory::HighlightSlot);
		OwnerInventory->OnSwitchedActivePanel.AddDynamic(this, &UVendorInventory::SwitchInventoryPanel);
		OwnerInventory->OnChangedOwnerGold.AddDynamic(this, &UVendorInventory::SetGoldValue);
	}
}

void UVendorInventory::UnbindEventDispatchers()
{
	if (IsValid(OwnerInventory))
	{
		OwnerInventory->OnRefreshInventory.RemoveDynamic(this, &UVendorInventory::RefreshInventory);
		OwnerInventory->OnHighlightInventorySlot.RemoveDynamic(this, &UVendorInventory::HighlightSlot);
		OwnerInventory->OnSwitchedActivePanel.RemoveDynamic(this, &UVendorInventory::SwitchInventoryPanel);
		OwnerInventory->OnChangedOwnerGold.RemoveDynamic(this, &UVendorInventory::SetGoldValue);
	}
}

void UVendorInventory::BuildInventorySlots(TArray<FItemData*> ItemData, int32 Size, UUniformGridPanel* GridPanel)
{
	int32 CurrentRow = 0;
	int32 CurrentColumn = 0;

	GridPanel->ClearChildren();

	for (int i = 0; i < Size; ++i)
	{
		FItemData* CurrentItemData = new FItemData();
		if (ItemData.IsValidIndex(i))
			CurrentItemData = ItemData[i];
		else
			CurrentItemData->Index = i;

		UVendorSlot* VendorSlot = CreateWidget<UVendorSlot>(GetOwningPlayer(), VendorSlotClass);
		VendorSlot->InitializeSlot(this, OwnerInventory, PlayerInventory, WidgetManager, i);
		VendorSlot->OverwriteSlot(CurrentItemData);

		UUniformGridSlot* CurrentSlot = GridPanel->AddChildToUniformGrid(VendorSlot, CurrentRow, CurrentColumn);
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

void UVendorInventory::SetOwnerInventory()
{
	UInteractableComponent* InteractableComponent = InventoryUtility::GetCurrentInteractableObject(GetOwningPlayer());
	auto InteractableData = InventoryUtility::GetInventoryFromInteractable(InteractableComponent);
	if (InteractableData.Get<0>())
		OwnerInventory = InteractableData.Get<1>();
}

void UVendorInventory::ResetSlotFocus()
{
	if (WidgetManager->GetCurrentlyFocusedWidget() == EWidgetType::Vendor)
	{
		SetFocusToSlot(FocusedSlot);
	}
}

void UVendorInventory::CreateInventoryP1()
{
	if (IsValid(OwnerInventory))
	{
		auto InventoryData = OwnerInventory->GetInventoryAndSize(EInventoryPanels::P1);
		BuildInventorySlots(InventoryData.Get<0>(), InventoryData.Get<1>(), InventoryUG1);
	}
}

void UVendorInventory::SetGoldValue()
{
	if (IsValid(OwnerInventory))
	{
		TextGold->SetText(UKismetTextLibrary::Conv_DoubleToText(OwnerInventory->GetOwnerGold(), HalfToEven));
	}
}

void UVendorInventory::SetFocusToSlot(int32 Index)
{
	auto ChildWidget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(Index);
	if (IsValid(ChildWidget))
	{
		FocusedSlot = Index;
		ChildWidget->SetKeyboardFocus();
	}
}

void UVendorInventory::HighlightSlot(int32 Index)
{
	UWidget* Widget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(Index);
	if (IsValid(Widget))
	{
		UVendorSlot* VendorSlot = Cast<UVendorSlot>(Widget);
		VendorSlot->HighlightSlot();
	}
}

void UVendorInventory::ScrollToSlot(UVendorSlot* VendorSlot) const
{
	ScrollBoxSlots->ScrollWidgetIntoView(VendorSlot, true, EDescendantScrollDestination::Center, 0);
}

void UVendorInventory::RefreshInventory(EInventoryPanels Panels)
{
	if (Panels != EInventoryPanels::None)
	{
		CreateInventoryP1();
		ResetSlotFocus();
	}
}

void UVendorInventory::SwitchInventoryPanel(EInventoryPanels Panel)
{
	if (GetActivePanel() != Panel)
	{
		InventoryUtility::PlaySoundOnTabSwitched();
		PanelSwitcher->SetActiveWidget(GetUniformGridFromPanel(Panel));
		OwnerInventory->SwitchActivePanel(Panel);
	}
}

void UVendorInventory::AssignCurrentlyFocusedSlot(UUserWidget* ChildWidget)
{
	if (IsValid(ChildWidget))
	{
		FocusedSlot = GetUniformGridFromPanel(GetActivePanel())->GetChildIndex(ChildWidget);
	}
}

void UVendorInventory::DisplaySampleSlots(int32 Slots)
{
	int32 CurrentRow = 0;
	int32 CurrentColumn = 0;
	UUniformGridPanel* CurrentPanel = GetUniformGridFromPanel(EInventoryPanels::P1);
	CurrentPanel->ClearChildren();
	for (int i = 0; i < Slots; ++i)
	{
		UVendorSlot* NewUStorageSlot = CreateWidget<UVendorSlot>(this, VendorSlotClass);
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

EInventoryPanels UVendorInventory::GetActivePanel() const
{
	if (PanelSwitcher->GetActiveWidget() == InventoryUG1)
		return EInventoryPanels::P1;

	//TODO Check other Panels
	return EInventoryPanels::P1;
}

UUniformGridPanel* UVendorInventory::GetUniformGridFromPanel(EInventoryPanels Panel)
{
	if (Panel == EInventoryPanels::P1)
		return InventoryUG1;

	//TODO Check other Panels
	return InventoryUG1;
}
