#include "BaseItemPickUp.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

ABaseItemPickUp::ABaseItemPickUp()
{
	InventoryCore = CreateDefaultSubobject<UInventoryCore>(TEXT("InventoryCore"));
}

void ABaseItemPickUp::Interaction(AActor* Interactor)
{
	Super::Interaction(Interactor);
	if (IsValid(Interactor))
		CachedInteractor = Interactor;
}

void ABaseItemPickUp::EndInteraction(AActor* Interactor)
{
	Super::EndInteraction(Interactor);
	if (IsValid(Interactor))
		CachedInteractor = Interactor;
}

void ABaseItemPickUp::ClientStartInteraction(AActor* Interactor)
{
	Super::EndInteraction(Interactor);
	if (IsValid(Interactor))
		CachedLocalInteractor = Interactor;

	if (InteractableComponent->IsShowLootBar())
	{
		InteractableComponent->ToggleInteractionWidget(false);
		
		auto PlayerInventory = InventoryUtility::GetPlayerInventory(CachedLocalInteractor);
		if (IsValid(PlayerInventory))
			PlayerInventory->OpenLootBarWidget();
	}
	else
		InventoryUtility::TakeAllItemsFromInventory(CachedLocalInteractor, InventoryCore);
}

void ABaseItemPickUp::ClientEndInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
		CachedLocalInteractor = Interactor;

	if (InteractableComponent->IsShowLootBar())
	{
		InteractableComponent->ToggleInteractionWidget(true);

		auto PlayerInventory = InventoryUtility::GetPlayerInventory(CachedLocalInteractor);
		if (IsValid(PlayerInventory))
			PlayerInventory->CloseLootBarWidget();
	}
}

void ABaseItemPickUp::RemoveInteraction()
{
	Super::RemoveInteraction();
	OnRemoveInteraction();
}

void ABaseItemPickUp::Destroyed()
{
	Super::Destroyed();
	OnRemoveInteraction();
}

void ABaseItemPickUp::GameLoad()
{
	SetPhysicsSimulationAndCollision();
}

void ABaseItemPickUp::MulticastSetPhysicsSimulation() const
{
	SetPhysicsSimulationAndCollision();
}

void ABaseItemPickUp::SetPhysicsSimulationAndCollision() const
{
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseItemPickUp::OnRemoveInteraction() const
{
	InteractableComponent->ToggleInteractionWidget(false);
	auto PlayerInventory = InventoryUtility::GetPlayerInventory(CachedLocalInteractor);

	if (InteractableComponent->IsShowLootBar() && IsValid(PlayerInventory))
		PlayerInventory->CloseLootBarWidget();
}
