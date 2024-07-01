#include "BaseItemPickUp.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

void ABaseItemPickUp::Interaction(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "FFFFFFFFFFFFFFF");
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

	if(InteractableComponent->IsShowLootBar())
	{
		InteractableComponent->ToggleInteractionWidget(false);
		// if(IsValid(CachedLocalInteractor->GetPlayerInventory()))
		// {
		// 	CachedLocalInteractor->GetPlayerInventory()->OpenLocalBarWidget();
		// }
	}
	else
	{
	//	InteractableComponent->TakeAllItemsFromInventory();
	}
}

void ABaseItemPickUp::ClientEndInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
		CachedLocalInteractor = Interactor;

	if(InteractableComponent->IsShowLootBar())
	{
		InteractableComponent->ToggleInteractionWidget(true);
		// if(IsValid(CachedLocalInteractor->GetPlayerInventory()))
		// {
		// 	CachedLocalInteractor->GetPlayerInventory()->CloseLocalBarWidget();
		// }
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

void ABaseItemPickUp::OnRemoveInteraction()
{
	InteractableComponent->ToggleInteractionWidget(false);
	// if(IsValid(CachedLocalInteractor->GetPlayerInventory()))
	// {
	// 	CachedLocalInteractor->GetPlayerInventory()->CloseLocalBarWidget();
	// }
}
