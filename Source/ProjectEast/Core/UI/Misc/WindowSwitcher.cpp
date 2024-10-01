#include "WindowSwitcher.h"
#include "SwitcherButton.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void UWindowSwitcher::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetWidgetButtonStyle();
}

void UWindowSwitcher::NativeConstruct()
{
	Super::NativeConstruct();
	WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager();

	InitializeButtons();
	SetWidgetButtonStyle();
}

void UWindowSwitcher::SetWidgetButtonStyle() const
{
	auto Canvas = GetSwitcherPanel();
	if (IsValid(Canvas))
		SwitcherWidgetPanels->SetActiveWidget(Canvas);
}

void UWindowSwitcher::InitializeButtons() const
{
	switch (ActiveWidget)
	{
	case EWidgetType::Inventory:
	case EWidgetType::PlayerCrafting:
	case EWidgetType::StationCrafting:
	case EWidgetType::Abilities:
		{
			InventoryButtonCrafting->InitializeButton(WidgetManager);
			InventoryButtonInventory->InitializeButton(WidgetManager);
			InventoryButtonAbilities->InitializeButton(WidgetManager);
			InventoryButtonMap->InitializeButton(WidgetManager);
			InventoryButtonQuests->InitializeButton(WidgetManager);
		}
		break;
	case EWidgetType::Vendor:
		{
			VendorButtonVendor->InitializeButton(WidgetManager);
			VendorButtonEquipment->InitializeButton(WidgetManager);
		}
		break;
	case EWidgetType::Storage:
		{
			StorageButtonStorage->InitializeButton(WidgetManager);
			StorageButtonEquipment->InitializeButton(WidgetManager);
		}
		break;
	default:
		;
	}
}

UCanvasPanel* UWindowSwitcher::GetSwitcherPanel() const
{
	switch (ActiveWidget)
	{
	case EWidgetType::Inventory:
	case EWidgetType::PlayerCrafting:
	case EWidgetType::StationCrafting:
	case EWidgetType::Abilities:
		return InventoryPanel;
	case EWidgetType::Vendor:
		return VendorPanel;
	case EWidgetType::Storage:
		return StoragePanel;
	case EWidgetType::LoadGame:
		return LoadGamePanel;
	default:
		return nullptr;
	}
}

