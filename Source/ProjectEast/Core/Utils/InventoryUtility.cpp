#include "InventoryUtility.h"

#include "ProjectEast/Core/Actors/Interactive/ObjectInteraction.h"
#include "ProjectEast/Core/Components/Interactive/InteractionComponent.h"

UInteractableComponent* InventoryUtility::GetCurrentInteractableObject(AActor* OwningPlayer)
{
	UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(
		OwningPlayer->GetComponentByClass(UInteractionComponent::StaticClass()));

	if (IsValid(InteractionComponent) && IsValid(InteractionComponent->GetInteractableComponent()))
	{
		return InteractionComponent->GetInteractableComponent();
	}

	return nullptr;
}
