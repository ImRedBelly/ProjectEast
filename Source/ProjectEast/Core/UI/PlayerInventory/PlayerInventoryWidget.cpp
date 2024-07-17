#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "PlayerInventoryWidget.h"
#include "Components/UniformGridSlot.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Actors/Interfaces/GamepadControls.h"
#include "ProjectEast/Core/Actors/Interfaces/IWidgetManager.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"

void UPlayerInventoryWidget::AssignCurrentlyFocusedSlot(UPlayerInventorySlot* PlayerInventorySlot)
{
	FocusedSlot = GetUniformGridFromPanel(GetActivePanel())->GetChildIndex(PlayerInventorySlot);
}

void UPlayerInventoryWidget::ScrollToSlot(UPlayerInventorySlot* PlayerInventorySlot) const
{
	ScrollBox->ScrollWidgetIntoView(PlayerInventorySlot, true,EDescendantScrollDestination::Center, 0.0f);
}

void UPlayerInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	CachedPlayerInventory = CachedPlayerController->GetPlayerInventory();
	CachedPlayerEquipment = CachedPlayerController->GetPlayerEquipment();

	PlayAnimation(AnimationConstruct, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);

	BindEventDispatchers();
	SetReceiverInventory();
	CreateInventoryP1();
	CreateInventoryP2();
	CreateInventoryP3();
	CreateInventoryP4();
	SetActivePanel();
	SetInitialInputDelayForSlot();
}

void UPlayerInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
	UnbindEventDispatchers();
	if (CachedPlayerInventory->IsRefreshOmClosingWidget())
		CachedPlayerInventory->ServerSortInventory(CachedPlayerInventory,
		                                           ESortMethod::Quicksort, EInventoryPanels::P1, true);
}

void UPlayerInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	DisplaySampleSlots(SampleSlotsToDisplay);
}

FReply UPlayerInventoryWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UPlayerInventoryWidget::BindEventDispatchers()
{
	CachedPlayerInventory->OnRefreshInventory.AddDynamic(this, &UPlayerInventoryWidget::RefreshInventory);
	CachedPlayerInventory->OnHighlightInventorySlot.AddDynamic(this, &UPlayerInventoryWidget::HighlightSlot);
	CachedPlayerInventory->OnSwitchedActivePanel.AddDynamic(this, &UPlayerInventoryWidget::SwitchInventoryPanel);
	SortWindow->OnEndSorting.AddDynamic(this, &UPlayerInventoryWidget::OnSortingFinished);
}

void UPlayerInventoryWidget::UnbindEventDispatchers()
{
	if (IsValid(CachedPlayerInventory))
	{
		CachedPlayerInventory->OnRefreshInventory.RemoveDynamic(this, &UPlayerInventoryWidget::RefreshInventory);
		CachedPlayerInventory->OnHighlightInventorySlot.RemoveDynamic(this, &UPlayerInventoryWidget::HighlightSlot);
		CachedPlayerInventory->OnSwitchedActivePanel.RemoveDynamic(this, &UPlayerInventoryWidget::SwitchInventoryPanel);
		SortWindow->OnEndSorting.RemoveDynamic(this, &UPlayerInventoryWidget::OnSortingFinished);
	}
}

void UPlayerInventoryWidget::RefreshInventory(EInventoryPanels Panel)
{
	switch (Panel)
	{
	case EInventoryPanels::P1:
		CreateInventoryP1();
		break;
	case EInventoryPanels::P2:
		CreateInventoryP2();
		break;
	case EInventoryPanels::P3:
		CreateInventoryP3();
		break;
	case EInventoryPanels::P4:
		CreateInventoryP4();
		break;
	default: ;
	}

	ResetSlotFocus();
}

void UPlayerInventoryWidget::SetActivePanel()
{
	ButtonPanel1->OnClickedButtonPanel.AddDynamic(this, &UPlayerInventoryWidget::SwitchInventoryPanel);
	ButtonPanel2->OnClickedButtonPanel.AddDynamic(this, &UPlayerInventoryWidget::SwitchInventoryPanel);
	ButtonPanel3->OnClickedButtonPanel.AddDynamic(this, &UPlayerInventoryWidget::SwitchInventoryPanel);
	ButtonPanel4->OnClickedButtonPanel.AddDynamic(this, &UPlayerInventoryWidget::SwitchInventoryPanel);

	ButtonPanel1->RefreshPanelButton(DefaultPanel);
	ButtonPanel2->RefreshPanelButton(DefaultPanel);
	ButtonPanel3->RefreshPanelButton(DefaultPanel);
	ButtonPanel4->RefreshPanelButton(DefaultPanel);

	PanelSwitcher->SetActiveWidget(GetUniformGridFromPanel(DefaultPanel));
	CachedPlayerInventory->SwitchedActivePanel(DefaultPanel);
	SetPanelTitle(DefaultPanel);
}

void UPlayerInventoryWidget::OnSortingFinished()
{
	if (IsUsingGamepad())
		SetFocusToSlot(0);
}

void UPlayerInventoryWidget::SetReceiverInventory()
{
	auto DataInventory = InventoryUtility::GetInventoryFromInteractable(
		InventoryUtility::GetCurrentInteractableObject(GetOwningPlayer()));
	CachedReceiverInventory = DataInventory.Get<1>();
}

void UPlayerInventoryWidget::SetInitialInputDelayForSlot() const
{
	if (IsUsingGamepad())
	{
		UPlayerInventorySlot* FirstSlot = Cast<UPlayerInventorySlot>(
			GetUniformGridFromPanel(GetActivePanel())->GetChildAt(0));
		FirstSlot->OnInitialInput();
	}
}

void UPlayerInventoryWidget::ResetSlotFocus()
{
	if (IGamepadControls* GamepadControls = Cast<IGamepadControls>(GetOwningPlayer()))
		if (GamepadControls->GetCurrentlyFocusedWidget() == EWidgetType::Inventory)
			SetFocusToSlot(FocusedSlot);
}

void UPlayerInventoryWidget::CreateInventoryP1()
{
	auto DataInventory = CachedPlayerInventory->GetInventoryAndSize(EInventoryPanels::P1);
	BuildInventorySlots(DataInventory.Get<0>(), DataInventory.Get<1>(), InventoryUG1);
}

void UPlayerInventoryWidget::CreateInventoryP2()
{
	auto DataInventory = CachedPlayerInventory->GetInventoryAndSize(EInventoryPanels::P2);
	BuildInventorySlots(DataInventory.Get<0>(), DataInventory.Get<1>(), InventoryUG2);
}

void UPlayerInventoryWidget::CreateInventoryP3()
{
	auto DataInventory = CachedPlayerInventory->GetInventoryAndSize(EInventoryPanels::P3);
	BuildInventorySlots(DataInventory.Get<0>(), DataInventory.Get<1>(), InventoryUG3);
}

void UPlayerInventoryWidget::CreateInventoryP4()
{
	auto DataInventory = CachedPlayerInventory->GetInventoryAndSize(EInventoryPanels::P4);
	BuildInventorySlots(DataInventory.Get<0>(), DataInventory.Get<1>(), InventoryUG4);
}

void UPlayerInventoryWidget::SetFocusToSlot(uint32 SlotIndex) const
{
	if (IsAnyPopupActive())
	{
		if (UWidget* Widget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(SlotIndex))
			Widget->SetKeyboardFocus();
	}
}

void UPlayerInventoryWidget::HighlightSlot(uint32 SlotIndex)
{
	if (UPlayerInventorySlot* CurrentSlot = Cast<UPlayerInventorySlot>(
		GetUniformGridFromPanel(GetActivePanel())->GetChildAt(SlotIndex)))
		CurrentSlot->HighlightSlot();
}

void UPlayerInventoryWidget::BuildInventorySlots(TArray<FItemData*> ItemData, int32 Size, UUniformGridPanel* GridPanel)
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


		UPlayerInventorySlot* InventorySLot = CreateWidget<UPlayerInventorySlot>(GetOwningPlayer(), DefaultPlayerInventorySlot);
		InventorySLot->InitializeSlot(CurrentItemData, CachedReceiverInventory, this, CachedPlayerEquipment,
		                              CachedPlayerInventory, DraggedImageSize, i);

		UUniformGridSlot* CurrentSlot = GridPanel->AddChildToUniformGrid(InventorySLot, CurrentRow, CurrentColumn);
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

EInventoryPanels UPlayerInventoryWidget::GetActivePanel() const
{
	if (PanelSwitcher->GetActiveWidget() == InventoryUG1)
		return EInventoryPanels::P1;
	if (PanelSwitcher->GetActiveWidget() == InventoryUG2)
		return EInventoryPanels::P2;
	if (PanelSwitcher->GetActiveWidget() == InventoryUG3)
		return EInventoryPanels::P3;
	if (PanelSwitcher->GetActiveWidget() == InventoryUG4)
		return EInventoryPanels::P4;
	return EInventoryPanels::P1;
}

UUniformGridPanel* UPlayerInventoryWidget::GetUniformGridFromPanel(EInventoryPanels Panel) const
{
	switch (Panel)
	{
	case EInventoryPanels::P1:
		return InventoryUG1;
	case EInventoryPanels::P2:
		return InventoryUG2;
	case EInventoryPanels::P3:
		return InventoryUG3;
	case EInventoryPanels::P4:
		return InventoryUG4;
	default:
		return InventoryUG1;
	}
}

void UPlayerInventoryWidget::SwitchInventoryPanel(EInventoryPanels NewPanel)
{
	if (GetActivePanel() != NewPanel)
	{
		InventoryUtility::PlaySoundOnTabSwitched();
		PanelSwitcher->SetActiveWidget(GetUniformGridFromPanel(NewPanel));
		CachedPlayerInventory->SwitchedActivePanel(NewPanel);

		ButtonPanel1->RefreshPanelButton(NewPanel);
		ButtonPanel2->RefreshPanelButton(NewPanel);
		ButtonPanel3->RefreshPanelButton(NewPanel);
		ButtonPanel4->RefreshPanelButton(NewPanel);

		SetPanelTitle(NewPanel);

		if (IGamepadControls* GamepadControls = Cast<IGamepadControls>(GetOwningPlayer()))
			if (GamepadControls->GetCurrentlyFocusedWidget() == EWidgetType::Inventory)
				SetFocusToSlot(0);
			else
				ScrollBox->ScrollToStart();
	}
}

void UPlayerInventoryWidget::SwitchToNextInventoryPanel(bool ToTheRight)
{
	if (ToTheRight)
	{
		switch (GetActivePanel())
		{
		case EInventoryPanels::P1:
			SwitchInventoryPanel(EInventoryPanels::P2);
			break;
		case EInventoryPanels::P2:
			SwitchInventoryPanel(EInventoryPanels::P3);
			break;
		case EInventoryPanels::P3:
			SwitchInventoryPanel(EInventoryPanels::P4);
			break;
		case EInventoryPanels::P4:
			SwitchInventoryPanel(EInventoryPanels::P1);
			break;
		default: ;
		}
	}
	else
	{
		switch (GetActivePanel())
		{
		case EInventoryPanels::P1:
			SwitchInventoryPanel(EInventoryPanels::P4);
			break;
		case EInventoryPanels::P2:
			SwitchInventoryPanel(EInventoryPanels::P1);
			break;
		case EInventoryPanels::P3:
			SwitchInventoryPanel(EInventoryPanels::P2);
			break;
		case EInventoryPanels::P4:
			SwitchInventoryPanel(EInventoryPanels::P3);
			break;
		default: ;
		}
	}
}

void UPlayerInventoryWidget::SetPanelTitle(EInventoryPanels Panel)
{
	FString Title = "";
	switch (Panel)
	{
	case EInventoryPanels::P1:
		Title = P1Title;
		break;
	case EInventoryPanels::P2:
		Title = P2Title;
		break;
	case EInventoryPanels::P3:
		Title = P3Title;
		break;
	case EInventoryPanels::P4:
		Title = P4Title;
		break;
	default: ;
	}
	PanelName->SetText(FText::FromString(Title));
}

void UPlayerInventoryWidget::DisplaySampleSlots(int32 IndexSlot)
{
	if (!IsValid(DefaultPlayerInventorySlot))
		return;

	uint32 CurrentRow = 0;
	uint32 CurrentColumn = 0;
	auto CurrentPanel = GetUniformGridFromPanel(EInventoryPanels::P1);
	CurrentPanel->ClearChildren();
	for (int i = 0; i < IndexSlot; ++i)
	{
		UPlayerInventorySlot* NewPlayerInventorySlot = CreateWidget<UPlayerInventorySlot>(this, DefaultPlayerInventorySlot);
		UUniformGridSlot* CurrentSlot = CurrentPanel->AddChildToUniformGrid(NewPlayerInventorySlot, CurrentRow, CurrentColumn);
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

bool UPlayerInventoryWidget::IsUsingGamepad() const
{
	if (IsValid(CachedPlayerController))
		return CachedPlayerController->IsUsingGamepad();
	return false;
}

bool UPlayerInventoryWidget::IsAnyPopupActive() const
{
	if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
		return WidgetManager->GetCurrentPopupType() != EWidgetType::Vendor;

	return false;
}
