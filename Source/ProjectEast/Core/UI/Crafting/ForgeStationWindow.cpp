#include "ForgeStationWindow.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/UI/Misc/Popups/PopupItemAdded.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"


void UForgeStationWindow::InitializeWindow(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore)
{
	Super::InitializeWindow(InPlayerCraftingCore, InCraftingCore);
	ButtonClose->OnClicked.AddDynamic(this, &UForgeStationWindow::CloseWidget);
	CraftingCore->OnItemCrafted.AddDynamic(this, &UForgeStationWindow::OnItemCrafted);
}

void UForgeStationWindow::NativeDestruct()
{
	ButtonClose->OnClicked.RemoveDynamic(this, &UForgeStationWindow::CloseWidget);
	CraftingCore->OnItemCrafted.AddDynamic(this, &UForgeStationWindow::OnItemCrafted);
	Super::NativeDestruct();
}

void UForgeStationWindow::OnItemCrafted(FCraftingData& CraftingData, AActor* OwnningPlayer)
{
	auto CraftableData = InventoryUtility::GetCraftableData(&CraftingData);
	if (InventoryUtility::IsItemClassValid(CraftableData.Get<0>()[0]))
	{
		PopupItemAdded->InitializeWidget(CraftableData.Get<0>()[0]);
	}
}

void UForgeStationWindow::CloseWidget()
{
	//TODO Remove Cast
	Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager()->CloseActiveWidget();
}
