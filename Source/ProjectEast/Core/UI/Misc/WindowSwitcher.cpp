#include "WindowSwitcher.h"
#include "SwitcherButton.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void UWindowSwitcher::NativeConstruct()
{
	Super::NativeConstruct();
	WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager();
	ButtonCrafting->InitializeButton(WidgetManager);
	ButtonInventory->InitializeButton(WidgetManager);
	ButtonAbilities->InitializeButton(WidgetManager);
	ButtonMap->InitializeButton(WidgetManager);
	ButtonQuests->InitializeButton(WidgetManager);

	SetWidgetButtonStyle();
}

void UWindowSwitcher::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWindowSwitcher::SetWidgetButtonStyle() const
{
	auto Canvas = GetSwitcherPanel(ActiveWidget);
	if (IsValid(Canvas))
	{
		SwitcherWidgetPanels->SetActiveWidget(Canvas);
	}
}

UCanvasPanel* UWindowSwitcher::GetSwitcherPanel(EWidgetType Type) const
{
	switch (Type)
	{
	case EWidgetType::Inventory:
	case EWidgetType::PlayerCrafting:
	case EWidgetType::StationCrafting:
	case EWidgetType::Abilities:
		return MainPanel;
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
