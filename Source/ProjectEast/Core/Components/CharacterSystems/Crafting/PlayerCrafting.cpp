#include "PlayerCrafting.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UPlayerCrafting::AssignCraftableData(FCraftingData* CraftingData)
{
	SelectedCraftingData = CraftingData;
	if (OnNewItemSelected.IsBound())
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
	UPlayerEquipment* PlayerEquipment = PlayerController->GetPlayerEquipment();
	if (IsValid(PlayerEquipment))
	{
		PlayerEquipment->CallOnItemDetach(*PreviewItemData);
		PlayerEquipment->CallOnItemAttach(*StoredPreviewData);


		if (IsValid(CurrentStationComponent) && CurrentStationComponent->GetCraftOnlyWhenWindowIsOpen())
		{
			CurrentStationComponent->ClearCraftingQueue(PlayerController);
			CurrentStationComponent->StopCraftingProcess(GetFirstItemFromQueue(), PlayerController);
		}

		CurrentStationComponent = nullptr;

		PreviewItemData = &EmptyItemData;
		StoredPreviewData = &EmptyItemData;
		bIsCraftingWidgetOpen = false;
		SelectedCraftingData = &EmptyCraftingData;
	}
	WidgetManager->CloseActiveWidget();
}

void UPlayerCrafting::OnItemUsed(FItemData& ItemData)
{
	if (InventoryUtility::IsItemClassValid(&ItemData))
	{
		if (ItemData.Class.GetDefaultObject()->UseType == EItemUseType::CraftingRecipe)
		{
			ClientUnlockCraftingRecipe(ItemData.Class.GetDefaultObject()->CraftingIDToUnlock);
		}
	}
}

void UPlayerCrafting::InitializeCrafting(AMainPlayerController* InPlayerController, UPlayerInventory* InPlayerInventory)
{
	ClientInitializeCrafting(InPlayerController, InPlayerInventory);
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
		CreateCraftingList();
}

void UPlayerCrafting::ClientInitializeCrafting(AMainPlayerController* InPlayerController, UPlayerInventory* InPlayerInventory)
{
	PlayerController = InPlayerController;
	PlayerInventory = InPlayerInventory;
	WidgetManager = PlayerController->GetWidgetManager();

	if (IsValid(PlayerInventory))
		PlayerInventory->OnItemUsed.AddDynamic(this, &UPlayerCrafting::OnItemUsed);
	
	CreateCraftingList();
}

bool UPlayerCrafting::UnlockCraftingRecipe(FString CraftingID)
{
	if (CraftingID != "" && !UnlockedRecipes.Contains(CraftingID))
	{
		UnlockedRecipes.Add(CraftingID);
		return true;
	}
	return false;
}

bool UPlayerCrafting::IsCraftingRecipeLocked(FCraftingData* CraftingData) const
{
	if (CraftingData->StartLocked)
		return !UnlockedRecipes.Contains(CraftingData->CraftingID);
	return false;
}

void UPlayerCrafting::FocusSelectedItem(FCraftingData* CraftingData)
{
	AssignCraftableData(CraftingData);
	AttachNewItemToPlayerPreview(SelectedCraftingData);
	if (OnRefreshed.IsBound())
		OnRefreshed.Broadcast();
}

FCraftingData* UPlayerCrafting::GetSelectedCraftingData() const
{
	return SelectedCraftingData;
}

void UPlayerCrafting::ReturnSecuredCraftingCost(FCraftingData* CraftingData, UCraftingCore* Station,
                                                AActor* OwningPlayer)
{
	if (IsValid(Station) && Station->GetCraftingCostMultiplier() > 0)
	{
		if (IsValid(PlayerInventory))
		{
			PlayerInventory->AddGoldToOwner(
				CraftingData->CraftingCounter * (CraftingData->CraftingCost * Station->GetCraftingCostMultiplier()));
		}
	}
}

TTuple<FItemData*, TArray<FSingleDTItem>> UPlayerCrafting::GetCurrentCraftableData() const
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

void UPlayerCrafting::AttachNewItemToPlayerPreview(FCraftingData* CraftingData)
{
	auto PlayerEquipment = PlayerController->GetPlayerEquipment();
	if (IsValid(PlayerEquipment))
	{
		if (InventoryUtility::IsItemClassValid(PreviewItemData))
		{
			PlayerEquipment->CallOnItemDetach(*PreviewItemData);
			PlayerEquipment->CallOnItemAttach(*StoredPreviewData);
		}
		auto CraftableData = InventoryUtility::GetCraftableData(CraftingData);

		PreviewItemData = CraftableData.Get<0>()[0];
		StoredPreviewData = PlayerEquipment->GetItemByEquipmentSlot(PreviewItemData->EquipmentSlot).Get<1>();
		PlayerEquipment->CallOnItemAttach(*PreviewItemData);
	}
}

int32 UPlayerCrafting::FindItemQuantity(FItemData* ItemData, APlayerController* Controller)
{
	if (IsValid(PlayerInventory))
	{
		auto Data = PlayerInventory->GetItemByData(ItemData);
		if (Data.Get<1>())
		{
			return Data.Get<0>()->Quantity;
		}
	}

	auto PlayerEquipment = PlayerController->GetPlayerEquipment();
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

void UPlayerCrafting::ServerTryToCraftItem(UCraftingCore* Sender, FCraftingData* CraftingData, AActor* OwningPlayer)
{
	auto CraftData = TryToCraftItem(CraftingData, Sender, OwningPlayer);
	if (CraftData.Get<0>())
		ClientTryToCraftReturnValue(CraftData.Get<0>(), CraftData.Get<1>(), Sender);
}

void UPlayerCrafting::ServerReturnSecuredMaterials(FCraftingData* CraftingData, UCraftingCore* Sender,
                                                   AActor* OwningPlayer)
{
	ReturnSecuredMaterials(CraftingData, OwningPlayer);
	ReturnSecuredCraftingCost(CraftingData, Sender, OwningPlayer);
	ClientRefreshUIData(Sender);
}

void UPlayerCrafting::ServerInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft,
                                                      UCraftingCore* Sender, AActor* OwningPlayer)
{
	auto InitData = TryToInitializeCraftingProcess(CraftingData, AmountToCraft, Sender, OwningPlayer);
	ClientInitializeCraftingProcessReturnValue(InitData.Get<0>(), CraftingData, AmountToCraft, Sender,
	                                           InitData.Get<1>());
}

void UPlayerCrafting::ClientTryToCraftReturnValue(bool ReturnValue, FText Message, UCraftingCore* Sender)
{
	if (ReturnValue)
	{
		Sender->ItemCrafted(PlayerController);
		if (OnRefreshed.IsBound())
			OnRefreshed.Broadcast();
	}
	else
	{
		WidgetManager->DisplayMessage(Message.ToString());
		Sender->StopCraftingProcess(GetFirstItemFromQueue(), PlayerController);
	}
}

TTuple<bool, FText> UPlayerCrafting::TryToInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft,
                                                                    UCraftingCore* Sender, AActor* OwningPlayer)
{
	auto InitData = Sender->CanInitializeCraftingProcess(CraftingData, AmountToCraft, OwningPlayer);
	if (InitData.Get<0>())
	{
		auto CraftableData = InventoryUtility::GetCraftableData(CraftingData);
		if (CanBeAddedToInventory(CraftableData.Get<0>(), OwningPlayer, Sender))
		{
			auto MaterialsData = SecureMaterialItems(CraftableData.Get<1>(), AmountToCraft, OwningPlayer);
			if (MaterialsData.Get<0>())
			{
				SecureCraftingCost(CraftingData, AmountToCraft, Sender, OwningPlayer);
				return MakeTuple(true, FText());
			}
			return MakeTuple(false, FText::FromString("Not enough materials."));
		}
		return MakeTuple(false, FText::FromString("Your Inventory is full."));
	}
	return MakeTuple(false, InitData.Get<1>());
}

void UPlayerCrafting::ClientInitializeCraftingProcessReturnValue(bool ReturnValue, FCraftingData* CraftingData,
                                                                 int32 AmountToCraft, UCraftingCore* Sender,
                                                                 FText Message) const
{
	if (ReturnValue)
	{
		Sender->InitializeCraftingProcess(CraftingData, AmountToCraft);
		if (OnRefreshed.IsBound())
			OnRefreshed.Broadcast();
	}
	else
	{
		WidgetManager->DisplayMessage(Message.ToString());
		Sender->FailedToInitializeCraftingProcess(CraftingData, AmountToCraft);
	}
}

void UPlayerCrafting::ClientRefreshUIData(UCraftingCore* Target) const
{
	if (Target->OnRefreshed.IsBound())
		Target->OnRefreshed.Broadcast();
}

void UPlayerCrafting::ClientUnlockCraftingRecipe(FString CraftingID)
{
	if (UnlockCraftingRecipe(CraftingID))
	{
		if (OnRefreshed.IsBound())
			OnRefreshed.Broadcast();
	}
}

void UPlayerCrafting::BeginPlay()
{
	//TODO Do Not Remove this Event
}


void UPlayerCrafting::BeginDestroy()
{
	if (IsValid(PlayerInventory))
		PlayerInventory->OnItemUsed.RemoveDynamic(this, &UPlayerCrafting::OnItemUsed);
	Super::BeginDestroy();
}

TTuple<bool, TArray<FItemData*>> UPlayerCrafting::SecureMaterialItems(TArray<FSingleDTItem> Materials,
                                                                      int32 AmountToCraft, AActor* OwningPlayer)
{
	if (AmountToCraft > 0)
	{
		if (IsValid(PlayerInventory))
		{
			bool bCanBeRemoved = false;
			TArray<FItemData*> SecuredItems;
			for (auto Material : Materials)
			{
				FItemData* FindItemData = Material.TableAndRow.DataTable->FindRow<FItemData>(
					Material.TableAndRow.RowName, "");
				if (InventoryUtility::IsItemClassValid(FindItemData))
				{
					auto ItemData = PlayerInventory->GetItemByData(FindItemData);

					int32 AmountToRemove = AmountToCraft * Material.Quantity;
					if (ItemData.Get<1>() && ItemData.Get<0>()->Quantity >= AmountToRemove)
					{
						FItemData* NewItemData = InventoryUtility::CopyItemData(ItemData.Get<0>());
						NewItemData->Quantity = AmountToRemove;
						SecuredItems.Add(NewItemData);
						bCanBeRemoved = true;
					}
					else
						break;
				}
				else
					break;
			}

			if (bCanBeRemoved)
			{
				for (auto SecuredItem : SecuredItems)
				{
					auto ItemData = PlayerInventory->GetItemByData(SecuredItem);
					PlayerInventory->RemoveItemQuantity(ItemData.Get<0>(), SecuredItem->Quantity);
				}

				return MakeTuple(true, SecuredItems);
			}
		}
	}
	return MakeTuple(false, TArray<FItemData*>());
}

TTuple<bool, FText> UPlayerCrafting::TryToCraftItem(FCraftingData* CraftingData, UCraftingCore* Sender,
                                                    AActor* OwningPlayer)
{
	if (InventoryUtility::IsCraftingDataValid(CraftingData))
	{
		if (IsValid(PlayerInventory))
		{
			auto CraftableData = InventoryUtility::GetCraftableData(CraftingData);
			if (!Sender->GetSpawnCraftedItem())
			{
				if (CanBeAddedToInventory(CraftableData.Get<0>(), OwningPlayer, Sender))
				{
					for (auto ItemData : CraftableData.Get<0>())
					{
						auto ItemByData = PlayerInventory->GetItemByData(ItemData);
						if (InventoryUtility::IsStackableAndHaveStacks(ItemData, 1))
						{
							if (ItemByData.Get<1>())
							{
								PlayerInventory->AddToStackInInventory(ItemData, ItemByData.Get<1>());
							}
							else
							{
								auto EmptySlotData = PlayerInventory->GetEmptyInventorySlot(ItemData);
								PlayerInventory->AddItemToInventoryArray(ItemData, EmptySlotData.Get<1>());
							}
						}
						else
						{
							int32 Qantity = FMathf::Clamp(ItemData->Quantity, 1, ItemData->Quantity);
							for (int i = 0; i < Qantity; ++i)
							{
								auto EmptySlotData = PlayerInventory->GetEmptyInventorySlot(ItemData);
								PlayerInventory->AddItemToInventoryArray(ItemData, EmptySlotData.Get<1>());
							}
						}

						PlayerInventory->AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(ItemData));
					}
					return MakeTuple(true, FText());
				}
				return MakeTuple(false, FText::FromString("Inventory is full."));
			}
			else
			{
				Sender->SpawnCraftedItem(CraftingData, OwningPlayer);
				return MakeTuple(true, FText());
			}
		}
		return MakeTuple(false, FText());
	}
	return MakeTuple(false, FText());
}

bool UPlayerCrafting::CanCraft(UCraftingCore* Station) const
{
	if (IsValid(Station))
	{
		if (Station->GetCraftOnlyWhenWindowIsOpen() && bIsCraftingWidgetOpen)
			return true;
	}
	return false;
}

void UPlayerCrafting::SecureCraftingCost(FCraftingData* CraftingData, int32 AmountToCraft,
                                         UCraftingCore* Station, AActor* OwningPlayer)
{
	if (IsValid(Station) && Station->GetCraftingCostMultiplier() > 0)
	{
		if (IsValid(PlayerInventory))
		{
			PlayerInventory->RemoveGoldFromOwner(
				AmountToCraft * (CraftingData->CraftingCost * Station->GetCraftingCostMultiplier()));
		}
	}
}

void UPlayerCrafting::ReturnSecuredMaterials(FCraftingData* CraftingData, AActor* OwningPlayer)
{
	auto CraftableData = InventoryUtility::GetCraftableData(CraftingData);
	for (auto RowName : CraftableData.Get<1>())
	{
		FItemData* FindItemData = RowName.TableAndRow.DataTable->FindRow<FItemData>(RowName.TableAndRow.RowName, "");
		FItemData* NewItemData = InventoryUtility::CopyItemData(FindItemData);
		NewItemData->Quantity = RowName.Quantity * CraftingData->CraftingCounter;

		if (IsValid(PlayerInventory))
		{
			auto InventoryData = PlayerInventory->GetInventoryAndSize(
				InventoryUtility::GetInventoryPanelFromItem(NewItemData));
			auto PartialStack = InventoryUtility::HasPartialStack(InventoryData.Get<0>(), NewItemData);

			if (PartialStack.Get<0>())
			{
				PlayerInventory->AddToStackInInventory(NewItemData, PartialStack.Get<1>());
			}
			else
			{
				PlayerInventory->AddItemToInventoryArray(NewItemData,
				                                         PlayerInventory->GetEmptyInventorySlot(NewItemData).Get<1>());
			}
		}
	}
}

bool UPlayerCrafting::CanBeAddedToInventory(TArray<FItemData*> CraftableItems, AActor* OwningPlayer,
                                            UCraftingCore* Station)
{
	if (!Station->GetSpawnCraftedItem())
	{
		if (IsValid(PlayerInventory))
		{
			bool bCanBeAdded = false;
			for (auto CraftableItem : CraftableItems)
			{
				if (InventoryUtility::IsStackableAndHaveStacks(CraftableItem, 0))
				{
					if (PlayerInventory->GetItemByData(CraftableItem).Get<1>())
						bCanBeAdded = true;
					else
					{
						if (PlayerInventory->GetEmptyInventorySlot(CraftableItem).Get<0>())
							bCanBeAdded = true;
						else
							return false;
					}
				}
				else
				{
					int32 Amount = PlayerInventory->GetAmountOfEmptySlots(
						InventoryUtility::GetInventoryPanelFromItem(CraftableItem));
					if (Amount >= CraftableItem->Quantity)
					{
						bCanBeAdded = true;
					}
					else
						return false;
				}
			}
			return bCanBeAdded;
		}
	}
	return true;
}
