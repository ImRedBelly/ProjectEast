#include "LootBag.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"

ALootBag::ALootBag()
{
    InventoryCore = CreateDefaultSubobject<UInventoryCore>(TEXT("InventoryCore"));
}

void ALootBag::Interaction(AActor* Interactor)
{
    if(IsValid(Interactor))
        MainInteractor = Interactor;
}

void ALootBag::EndInteraction(AActor* Interactor)
{
    if(IsValid(Interactor))
        MainInteractor = Interactor;
}

void ALootBag::RemoveInteraction()
{
    OnDestroyed();
}

void ALootBag::ClientStartInteraction(AActor* Interactor)
{
    if(IsValid(Interactor))
    {
        LocalInteractor = Interactor;
        if(InteractableComponent->IsShowLootBar())
        {
            InteractableComponent->ToggleInteractionWidget(false);
            auto PlayerInventory = InventoryUtility::GetPlayerInventory(LocalInteractor);
            if(IsValid(PlayerInventory))
                PlayerInventory->OpenLootBarWidget();
        }
        else
        {
            InventoryUtility::TakeAllItemsFromInventory(LocalInteractor, InventoryCore);
        }
    }
}

void ALootBag::ClientEndInteraction(AActor* Interactor)
{
    if(IsValid(Interactor))
    {
        LocalInteractor = Interactor;
        if(InteractableComponent->IsShowLootBar())
        {
            InteractableComponent->ToggleInteractionWidget(true);
            auto PlayerInventory = InventoryUtility::GetPlayerInventory(LocalInteractor);
            if(IsValid(PlayerInventory))
                PlayerInventory->CloseLootBarWidget();
        }
    }
}

void ALootBag::Destroyed()
{
    OnDestroyed();
    Super::Destroyed();
}

void ALootBag::OnDestroyed() const
{
    InteractableComponent->ToggleInteractionWidget(false);
    if(InteractableComponent->IsShowLootBar())
    {
        auto PlayerInventory = InventoryUtility::GetPlayerInventory(LocalInteractor);
        if(IsValid(PlayerInventory))
            PlayerInventory->CloseLootBarWidget();
    }
}

