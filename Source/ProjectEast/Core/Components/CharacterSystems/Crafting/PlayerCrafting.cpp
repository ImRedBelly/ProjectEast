#include "PlayerCrafting.h"

#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UPlayerCrafting::OpenCraftingWidget(UCraftingCore* CraftingCore, EWidgetType WidgetType)
{
	if (IsValid(CraftingCore))
		CurrentStationComponent = CraftingCore;
	else
		CurrentStationComponent = this;

	WidgetManager->OpenNewWidget(WidgetType);
	WidgetManager->InitializeCraftingWidgets(CurrentStationComponent);
	bIsCraftingWidgetOpen = true;
}

void UPlayerCrafting::CloseCraftingWidget()
{
	WidgetManager->CloseActiveWidget();
}

void UPlayerCrafting::InitializeCrafting(AMainPlayerController* InPlayerController)
{
	ClientInitializeCrafting(InPlayerController);
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
		CreateCraftingList();
}

void UPlayerCrafting::ClientInitializeCrafting(AMainPlayerController* InPlayerController)
{
	PlayerController = InPlayerController;
	WidgetManager = PlayerController->GetWidgetManager();
	CreateCraftingList();
}

void UPlayerCrafting::BeginPlay()
{
	//TODO Do Not Remove this Event
}
