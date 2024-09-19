#include "PlayerCrafting.h"

#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UPlayerCrafting::AssignCraftableData(FCraftingData* CraftingData)
{
	SelectedCraftingData = CraftingData;
	if(OnNewItemSelected.IsBound())
		OnNewItemSelected.Broadcast(*SelectedCraftingData);
}

void UPlayerCrafting::OpenCraftingWidget(UCraftingCore* CraftingCore, EWidgetType WidgetType)
{
	if (IsValid(CraftingCore))
		CurrentStationComponent = CraftingCore;
	else
		CurrentStationComponent = this;

	WidgetManager->InitializeCraftingStation(CurrentStationComponent);
	SetCurrentCraftingStation(CurrentStationComponent);
	WidgetManager->SwitchWidgetTo(WidgetType);
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

bool UPlayerCrafting::IsCraftingRecipeLocked(FCraftingData* CraftingData)
{
	if (CraftingData->StartLocked)
		return !UnlockedRecipes.Contains(CraftingData->CraftingID);
	return false;
}

void UPlayerCrafting::FocusSelectedItem(FCraftingData* CraftingData)
{
	AssignCraftableData(CraftingData);
	//AttachNewItemToPlayerPreview(SelectedCraftingData);
	if(OnRefreshed.IsBound())
		OnRefreshed.Broadcast();
}

TTuple<FItemData*, TArray<FSingleDTItem>> UPlayerCrafting::GetCurrentCraftableData()
{
	if (InventoryUtility::IsCraftingDataValid(SelectedCraftingData))
	{
		auto Data = InventoryUtility::GetCraftableData(SelectedCraftingData);
		return MakeTuple(Data.Get<0>()[0], Data.Get<1>());
	}
	return MakeTuple(new FItemData(), TArray<FSingleDTItem>());
}

void UPlayerCrafting::SetCurrentCraftingStation(UCraftingCore* StationComponent)
{
	CurrentStationComponent = StationComponent;
}

int32 UPlayerCrafting::FindItemQuantity(FItemData* ItemData, APlayerController* Controller)
{
	auto PlayerInventory = InventoryUtility::GetPlayerInventory(Controller);
	if (IsValid(PlayerInventory))
	{
		auto Data = PlayerInventory->GetItemByData(ItemData);
		if (Data.Get<1>())
		{
			return Data.Get<0>()->Quantity;
		}
	}

	auto PlayerEquipment = InventoryUtility::GetPlayerEquipment(Controller);
	if (IsValid(PlayerEquipment))
	{
		auto Data = PlayerEquipment->GetItemByID(ItemData->ID);
		if (Data.Get<0>())
		{
			return Data.Get<1>()->Quantity;
		}
	}
	return 0;
}

void UPlayerCrafting::BeginPlay()
{
	//TODO Do Not Remove this Event
}