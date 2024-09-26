#include "PlayerEquipment.h"

#include "PlayerInventory.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Actors/Inventory/BaseEquippable.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/PlayerLeveling.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"


void UPlayerEquipment::InitializeEquipment()
{
	ClientInitializeEquipment();
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		BuildEquipment();
		BuildInitialEquipment();
	}
}

void UPlayerEquipment::ClientInitializeEquipment()
{
	if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		ActorLeveling = Cast<UPlayerLeveling>(GetOwner());
		BuildEquipment();
	}
}

void UPlayerEquipment::BuildEquipment()
{
	for (auto i = 0; i <= static_cast<int32>(EItemSlot::Pocket4); ++i)
	{
		EItemSlot EnumValue = static_cast<EItemSlot>(i);
		if (EnumValue == EItemSlot::None)
			continue;

		if (EquipmentData.Find(EnumValue))
		{
			FItemData* NewItemData = EquipmentData[EnumValue];
			NewItemData->EquipmentSlot = EnumValue;
			NewItemData->bIsEquipped = true;
			EquipmentData[EnumValue] = NewItemData;
		}
		else
		{
			FItemData* NewItemData = new FItemData();
			NewItemData->EquipmentSlot = EnumValue;
			NewItemData->bIsEquipped = false;
			EquipmentData.Add(EnumValue, NewItemData);
		}
	}
}

void UPlayerEquipment::BuildInitialEquipment()
{
	for (int i = 0; i < InitialEquipment.Num(); ++i)
	{
		auto DataTable = InitialEquipment[i].TableAndRow.DataTable;
		auto RowName = InitialEquipment[i].TableAndRow.RowName;

		auto CurrentItemData = DataTable->FindRow<FItemData>(RowName,TEXT(""));
		FItemData* CopyItem = InventoryUtility::CopyItemData(CurrentItemData);
		CopyItem->Quantity = FMathf::Clamp(InitialEquipment[i].Quantity, 1, InitialEquipment[i].Quantity);

		auto AssignItem = AssignItemFromEquipmentSlot(CopyItem);
		AddItemToEquipmentArray(CopyItem, AssignItem->EquipmentSlot);
		AttachItemToEquipment(CopyItem);

		auto PlayerInventory = InventoryUtility::GetPlayerInventory(GetOwner());
		if (IsValid(PlayerInventory))
			PlayerInventory->AddWeightToInventory(InventoryUtility::CalculateStackedItemWeight(CopyItem));
	}
	InitialEquipment.Empty();
}

void UPlayerEquipment::ServerSpawnEquippableActor(FItemData* ItemData)
{
	SpawnAndAttachEquippableActor(ItemData);
}

void UPlayerEquipment::ServerDestroyEquippableActor(FItemData* ItemData)
{
	DestroyEquippableActor(ItemData);
}

void UPlayerEquipment::ClientUpdateAddedItem(FItemData* ItemData)
{
	UpdateAddedItem(ItemData);
}

void UPlayerEquipment::ClientUpdateRemovedItem(FItemData* ItemData)
{
	UpdateRemovedItem(ItemData);
}

void UPlayerEquipment::ServerTransferItemFromInventory(FItemData* ItemData, FItemData* InSlotData,
                                                       UInventoryCore* Inventory, EInputMethodType Method)
{
	auto TransferData = TransferItemFromInventoryToEquipment(ItemData, InSlotData, Inventory, Method);
	ClientTransferItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>());
}

void UPlayerEquipment::ServerTransferItemFromEquipment(FItemData* ItemData, FItemData* InItemData)
{
	TransferItemFromEquipmentToEquipment(ItemData, InItemData);
}

void UPlayerEquipment::ClientTransferItemReturnValue(bool Success, FText FailureMessage) const
{
	if (!Success)
	{
		UWidgetManager* WidgetManager = Cast<AMainPlayerController>(GetOwner())->GetWidgetManager();
		WidgetManager->DisplayMessage(FailureMessage.ToString());
	}
}

void UPlayerEquipment::ServerUpdateEquipmentMesh(EItemSlot Slot, USkeletalMesh* Mesh) const
{
	if (OnUpdateEquipmentMesh.IsBound())
		OnUpdateEquipmentMesh.Broadcast(Slot, Mesh);
}

void UPlayerEquipment::ServerToggleEquippableSocket(bool IsInCombat, EItemSlot Slot)
{
	ToggleEquippableSocket(IsInCombat, Slot);
}

void UPlayerEquipment::ServerToggleAllEquippableSockets(bool IsInCombat)
{
	for (auto KV : EquippableReferences)
		ToggleEquippableSocket(IsInCombat, KV.Key);
}

void UPlayerEquipment::UpdateAddedItem(FItemData* ItemData)
{
	EquipmentData.Add(ItemData->EquipmentSlot, ItemData);
	if (OnAddedToEquipment.IsBound())
		OnAddedToEquipment.Broadcast(*EquipmentData[ItemData->EquipmentSlot]);
	if (OnItemAttach.IsBound())
		OnItemAttach.Broadcast(*EquipmentData[ItemData->EquipmentSlot]);
}

void UPlayerEquipment::UpdateRemovedItem(FItemData* ItemData)
{
	EquipmentData.Remove(ItemData->EquipmentSlot);
	EquipmentData.Add(ItemData->EquipmentSlot, ItemData);
	if (OnRemovedFromEquipment.IsBound())
		OnRemovedFromEquipment.Broadcast(*EquipmentData[ItemData->EquipmentSlot]);
	if (OnItemDetach.IsBound())
		OnItemDetach.Broadcast(*EquipmentData[ItemData->EquipmentSlot]);
}

void UPlayerEquipment::CallOnItemAttach(FItemData& ItemData)
{
	if (OnItemAttach.IsBound())
		OnItemAttach.Broadcast(ItemData);
}

void UPlayerEquipment::CallOnItemDetach(FItemData& ItemData)
{
	if (OnItemDetach.IsBound())
		OnItemDetach.Broadcast(ItemData);
}

void UPlayerEquipment::AddItemToEquipmentArray(FItemData* ItemData, EItemSlot Slot)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		ItemData->EquipmentSlot = Slot;
		ItemData->bIsEquipped = true;

		EquipmentData.Add(Slot, ItemData);
		if (UKismetSystemLibrary::IsStandalone(GetWorld()))
		{
			if (OnAddedToEquipment.IsBound())
				OnAddedToEquipment.Broadcast(*ItemData);
		}
		else
			ClientUpdateAddedItem(ItemData);
	}
}

void UPlayerEquipment::RemoveItemFromEquipmentArray(FItemData* ItemData)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		auto CurrentSlot = ItemData->EquipmentSlot;

		EquipmentData.Remove(CurrentSlot);
		FItemData* EmptyItemData = new FItemData();
		EmptyItemData->EquipmentSlot = CurrentSlot;
		EquipmentData.Add(CurrentSlot, EmptyItemData);

		if (UKismetSystemLibrary::IsStandalone(GetWorld()))
		{
			if (OnRemovedFromEquipment.IsBound())
				OnRemovedFromEquipment.Broadcast(*ItemData);
		}
		else
			ClientUpdateRemovedItem(ItemData);
	}
}

void UPlayerEquipment::AddToStackInEquipment(FItemData* ItemData, EItemSlot Slot)
{
	if (EquipmentData.Contains(Slot))
	{
		FItemData* NewItemData = InventoryUtility::CopyItemData(EquipmentData[Slot]);
		NewItemData->Quantity = ItemData->Quantity + EquipmentData[Slot]->Quantity;
		AddItemToEquipmentArray(NewItemData, NewItemData->EquipmentSlot);
	}
}


void UPlayerEquipment::ServerRemoveItemQuantity(FItemData* ItemData, int16 Quantity)
{
	RemoveItemQuantity(ItemData, Quantity);
}


bool UPlayerEquipment::TryToAddToPartialStack(FItemData* ItemData)
{
	TArray<FItemData*> ItemsData;
	for (auto Data : EquipmentData)
	{
		auto a = Data.Value;
		ItemsData.Add(InventoryUtility::CopyItemData(a));
	}
	auto PartialStackData = InventoryUtility::HasPartialStack(ItemsData, ItemData);

	if (PartialStackData.Get<0>())
	{
		AddToStackInEquipment(ItemData, ItemsData[PartialStackData.Get<1>()]->EquipmentSlot);
		return true;
	}

	return false;
}

void UPlayerEquipment::AttachItemToEquipment(FItemData* ItemData)
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		if (OnItemAttach.IsBound())
			OnItemAttach.Broadcast(*ItemData);
		switch (ItemData->EquipmentSlot)
		{
		case EItemSlot::Head:
		case EItemSlot::Gloves:
		case EItemSlot::Neck:
		case EItemSlot::Chest:
		case EItemSlot::Legs:
		case EItemSlot::Boots:
			ServerUpdateEquipmentMesh(ItemData->EquipmentSlot, ItemData->Class.GetDefaultObject()->SkeletalMesh);
			break;
		case EItemSlot::Weapon:
		case EItemSlot::Shield:
			ServerSpawnEquippableActor(ItemData);
			break;
		case EItemSlot::None:
		case EItemSlot::Pocket1:
		case EItemSlot::Pocket2:
		case EItemSlot::Pocket3:
		case EItemSlot::Pocket4:
			break;
		}
	}
}

void UPlayerEquipment::DetachItemFromEquipment(FItemData* ItemData)
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		if (OnItemDetach.IsBound())
			OnItemDetach.Broadcast(*ItemData);
		switch (ItemData->EquipmentSlot)
		{
		case EItemSlot::Head:
		case EItemSlot::Gloves:
		case EItemSlot::Neck:
		case EItemSlot::Chest:
		case EItemSlot::Legs:
		case EItemSlot::Boots:
			ServerUpdateEquipmentMesh(ItemData->EquipmentSlot, nullptr);
			break;
		case EItemSlot::Weapon:
		case EItemSlot::Shield:
			ServerDestroyEquippableActor(ItemData);
			break;
		case EItemSlot::None:
		case EItemSlot::Pocket1:
		case EItemSlot::Pocket2:
		case EItemSlot::Pocket3:
		case EItemSlot::Pocket4:
			break;
		}
	}
}

void UPlayerEquipment::RemoveItemQuantity(FItemData* ItemData, int32 Quantity)
{
	if (ItemData->Quantity > Quantity)
	{
		RemoveItemFromEquipmentArray(ItemData);
		FItemData* NewItemData = InventoryUtility::CopyItemData(ItemData);
		NewItemData->Quantity = ItemData->Quantity - Quantity;
		AddItemToEquipmentArray(NewItemData, NewItemData->EquipmentSlot);
	}
	else if (ItemData->Quantity <= Quantity)
	{
		RemoveItemFromEquipmentArray(ItemData);
		DetachItemFromEquipment(ItemData);
	}
}

TTuple<bool, FText> UPlayerEquipment::CanItemBeEquipped(FItemData* ItemData) const
{
	if (bIsCheckRequiredLevel)
		if (IsValid(ActorLeveling))
			if (ActorLeveling->GetCurrentLevel() < ItemData->Class.GetDefaultObject()->RequiredLevel)
				return MakeTuple(false, FText::FromString("Higher Level required."));
	return MakeTuple(true, FText());
}

TTuple<bool, FItemData*> UPlayerEquipment::GetItemByEquipmentSlot(EItemSlot Slot) const
{
	if (EquipmentData.Contains(Slot))
	{
		FItemData* ItemData = InventoryUtility::CopyItemData(EquipmentData.FindRef(Slot));
		if (InventoryUtility::IsItemClassValid(ItemData))
			return MakeTuple(true, ItemData);
	}

	FItemData* NewItemData = new FItemData();
	NewItemData->EquipmentSlot = Slot;
	return MakeTuple(false, NewItemData);
}

TTuple<bool, FItemData*> UPlayerEquipment::GetItemByID(FString ItemID) const
{
	for (auto KV : EquipmentData)
		if (KV.Value->ID == ItemID)
			return MakeTuple(true, KV.Value);
	return MakeTuple(false, new FItemData());
}

TArray<FItemData*> UPlayerEquipment::GetEquipmentItems() const
{
	TArray<FItemData*> Elements;
	for (auto KV : EquipmentData)
		Elements.Add(KV.Value);
	return Elements;
}

ABaseEquippable* UPlayerEquipment::GetEquippedActorReference(EItemSlot Slot) const
{
	if (EquippableReferences.Contains(Slot))
		return EquippableReferences[Slot];
	return nullptr;
}

FItemData* UPlayerEquipment::AssignItemFromEquipmentSlot(FItemData* ItemData)
{
	TArray PocketSlots =
	{
		EItemSlot::Pocket1,
		EItemSlot::Pocket2,
		EItemSlot::Pocket3,
		EItemSlot::Pocket4,
	};

	if (PocketSlots.Contains(ItemData->EquipmentSlot))
	{
		auto FindData = InventoryUtility::FindEmptyEquipmentSlot(EquipmentData, PocketSlots, EItemSlot::Pocket1);
		return GetItemByEquipmentSlot(FindData.Get<1>()).Get<1>();
	}
	else
	{
		if (bIsEnableOffHand && ItemData->Class.GetDefaultObject()->WeaponType == EWeaponType::OneHand)
		{
			if (!GetItemByEquipmentSlot(EItemSlot::Shield).Get<0>())
			{
				TArray HandSlots =
				{
					EItemSlot::Weapon,
					EItemSlot::Shield,
				};

				auto FindData = InventoryUtility::FindEmptyEquipmentSlot(EquipmentData, HandSlots, EItemSlot::Weapon);
				return GetItemByEquipmentSlot(FindData.Get<1>()).Get<1>();
			}
			else
				return GetItemByEquipmentSlot(EItemSlot::Weapon).Get<1>();
		}
		else
		{
			AAA = GetItemByEquipmentSlot(ItemData->EquipmentSlot);
			return AAA.Get<1>();
		}
	}
}

void UPlayerEquipment::AttachEquippableToNewSocket(EItemSlot Slot, FName SocketName) const
{
	if (auto EquippedActor = GetEquippedActorReference(Slot))
		EquippedActor->EventAttachToNewSocket(SocketName);
}


void UPlayerEquipment::ToggleEquippableSocket(bool CombatMode, EItemSlot Slot) const
{
	if (auto EquippedActor = GetEquippedActorReference(Slot))
		EquippedActor->EventToggleCombatMode(CombatMode);
}

void UPlayerEquipment::SpawnAndAttachEquippableActor(FItemData* ItemData)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
	{
		if (InventoryUtility::IsItemClassValid(ItemData))
		{
			switch (ItemData->EquipmentSlot)
			{
			case EItemSlot::Weapon:
				{
					auto Reference = GetEquippedActorReference(ItemData->EquipmentSlot);
					if (IsValid(Reference))
						Reference->Destroy();

					FActorSpawnParameters Parameters;
					Parameters.Owner = GetOwner();
					Parameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
					Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;

					auto NewActor = GetWorld()->SpawnActor<ABaseEquippable>(
						ItemData->Class.GetDefaultObject()->EquippableClass, GetOwner()->GetActorTransform(),
						Parameters);
					NewActor->InitializeItem(ItemData->EquipmentSlot);
					EquippableReferences.Add(ItemData->EquipmentSlot, NewActor);
					NewActor->EventToggleCombatMode(true);
				}
				break;
			case EItemSlot::Shield:
				{
					auto Reference = GetEquippedActorReference(ItemData->EquipmentSlot);
					if (IsValid(Reference))
						Reference->Destroy();

					FActorSpawnParameters Parameters;
					Parameters.Owner = GetOwner();
					Parameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
					Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;

					auto NewActor = GetWorld()->SpawnActor<ABaseEquippable>(
						ItemData->Class.GetDefaultObject()->EquippableClass, GetOwner()->GetActorTransform(),
						Parameters);
					NewActor->InitializeItem(ItemData->EquipmentSlot);
					EquippableReferences.Add(ItemData->EquipmentSlot, NewActor);
					NewActor->EventToggleCombatMode(true);
				}
				break;
			}
		}
	}
}

void UPlayerEquipment::DestroyEquippableActor(FItemData* ItemData)
{
	if (InventoryUtility::SwitchHasOwnerAuthority(this))
		if (auto EquippedActor = GetEquippedActorReference(ItemData->EquipmentSlot))
		{
			EquippedActor->Destroy();
			EquippableReferences.Remove(ItemData->EquipmentSlot);
		}
}

void UPlayerEquipment::TryToUnequipAssociatedSlot(FItemData* ItemData, UInventoryCore* Inventory)
{
	EItemSlot AssociatedSlot = EItemSlot::None;
	if (bIsEnableOffHand)
	{
		switch (ItemData->EquipmentSlot)
		{
		case EItemSlot::Weapon:
			AssociatedSlot = EItemSlot::Shield;
			break;
		case EItemSlot::Shield:
			AssociatedSlot = EItemSlot::Weapon;
			break;
		}

		switch (ItemData->Class.GetDefaultObject()->WeaponType)
		{
		case EWeaponType::TwoHand:
			{
				auto Item = GetItemByEquipmentSlot(AssociatedSlot);
				if (Item.Get<0>())
				{
					auto TransferData = Inventory->TransferItemFromEquipment(
						Item.Get<1>(), new FItemData(), EInputMethodType::RightClick, this);
					Inventory->ClientTransferItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>());
				}
			}
			break;
		case EWeaponType::OneHand:
		case EWeaponType::OffHand:
			{
				auto Item = GetItemByEquipmentSlot(AssociatedSlot);
				if (Item.Get<0>())
				{
					if (ItemData->Class.GetDefaultObject()->WeaponType == EWeaponType::TwoHand)
					{
						auto TransferData = Inventory->TransferItemFromEquipment(
							Item.Get<1>(), nullptr, EInputMethodType::RightClick, this);
						Inventory->ClientTransferItemReturnValue(TransferData.Get<0>(), TransferData.Get<1>());
					}
				}
			}
			break;
		}
	}
}

TTuple<bool, FText> UPlayerEquipment::TransferItemFromInventoryToEquipment(
	FItemData* ItemData, FItemData* InSlotData, UInventoryCore* Inventory, EInputMethodType Method)
{
	FItemData* InSlotItemData = InSlotData;
	auto EquippedData = CanItemBeEquipped(ItemData);
	if (EquippedData.Get<0>())
	{
		if (Method == EInputMethodType::RightClick)
			InSlotItemData = AssignItemFromEquipmentSlot(ItemData);

		if (InventoryUtility::IsItemClassValid(InSlotItemData))
		{
			if (InventoryUtility::AreItemsTheSame(ItemData, InSlotItemData)
				&& InventoryUtility::AreItemsStackable(ItemData, InSlotItemData))
			{
				AddToStackInEquipment(ItemData, InSlotItemData->EquipmentSlot);
				Inventory->RemoveItemFromInventoryArray(ItemData);
			}
			else
			{
				RemoveItemFromEquipmentArray(InSlotItemData);
				Inventory->AddItemToInventoryArray(InSlotItemData, ItemData->Index);
				AddItemToEquipmentArray(ItemData, InSlotItemData->EquipmentSlot);
				DetachItemFromEquipment(InSlotItemData);
				AttachItemToEquipment(ItemData);
				TryToUnequipAssociatedSlot(ItemData, Inventory);
			}

			return MakeTuple(true, FText());
		}
		else
		{
			Inventory->RemoveItemFromInventoryArray(ItemData);
			AddItemToEquipmentArray(ItemData, InSlotItemData->EquipmentSlot);
			//AttachItemToEquipment(ItemData);
			TryToUnequipAssociatedSlot(ItemData, Inventory);
			return MakeTuple(true, FText());
		}
	}
	return MakeTuple(false, EquippedData.Get<1>());
}

void UPlayerEquipment::TransferItemFromEquipmentToEquipment(FItemData* DraggedData, FItemData* InSlotData)
{
	if (InventoryUtility::IsItemClassValid(InSlotData))
	{
		RemoveItemFromEquipmentArray(InSlotData);
		AddItemToEquipmentArray(DraggedData, InSlotData->EquipmentSlot);
		RemoveItemFromEquipmentArray(DraggedData);
		AttachItemToEquipment(InSlotData);
		AttachItemToEquipment(DraggedData);
	}
	else
	{
		RemoveItemFromEquipmentArray(DraggedData);
		DetachItemFromEquipment(DraggedData);
		AddItemToEquipmentArray(DraggedData, InSlotData->EquipmentSlot);
		AttachItemToEquipment(DraggedData);
	}
}
