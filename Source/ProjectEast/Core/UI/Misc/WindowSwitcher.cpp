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
}

void UWindowSwitcher::NativeDestruct()
{
	Super::NativeDestruct();
}


