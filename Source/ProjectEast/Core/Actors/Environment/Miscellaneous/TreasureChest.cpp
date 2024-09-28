#include "TreasureChest.h"

#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

ATreasureChest::ATreasureChest()
{
	InventoryCore = CreateDefaultSubobject<UInventoryCore>(TEXT("InventoryCore"));
}

void ATreasureChest::RemoveInteraction()
{
	OnDestroyed();
}

void ATreasureChest::ClientStartInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		bIsOpen = true;
		OnAnimateOpenChest(true);
        LocalInteractor = Interactor;
		if(InteractableComponent->IsShowLootBar())
		{
			InteractableComponent->ToggleInteractionWidget(false);
			auto PlayerInventory = InventoryUtility::GetPlayerInventory(Interactor);
			if(IsValid(PlayerInventory))
				PlayerInventory->OpenLootBarWidget();
		}
		else
		{
			InventoryUtility::TakeAllItemsFromInventory(Interactor, InventoryCore);
		}
	}
}

void ATreasureChest::ClientEndInteraction(AActor* Interactor)
{
	if(IsValid(Interactor) && bIsOpen)
	{
		bIsOpen = false;
		OnAnimateOpenChest(false);
        LocalInteractor = Interactor;
		if(InteractableComponent->IsShowLootBar())
		{
			InteractableComponent->ToggleInteractionWidget(true);
			auto PlayerInventory = InventoryUtility::GetPlayerInventory(Interactor);
			if(IsValid(PlayerInventory))
				PlayerInventory->CloseLootBarWidget();
		}
	}
}


void ATreasureChest::Destroyed()
{
	OnDestroyed();
	Super::Destroyed();
}

void ATreasureChest::OnDestroyed() const
{
	InteractableComponent->ToggleInteractionWidget(false);
	if(InteractableComponent->IsShowLootBar())
	{
		auto PlayerInventory = InventoryUtility::GetPlayerInventory(LocalInteractor);
		if(IsValid(PlayerInventory))
			PlayerInventory->CloseLootBarWidget();
	}
}

