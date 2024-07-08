#include "Interactable.h"

void IInteractable::Initialize()
{
}

void IInteractable::Interaction(AActor* Interactor)
{
}

void IInteractable::EndInteraction(AActor* Interactor)
{
}

void IInteractable::RemoveInteraction()
{
}

void IInteractable::GameLoad()
{
}

void IInteractable::ClientStartInteraction(AActor* Interactor)
{
}

void IInteractable::ClientEndInteraction(AActor* Interactor)
{
}

void IInteractable::ClientPreInteraction(AActor* Interactor)
{
}

bool IInteractable::CanBeInteractedWith()
{
	return true;
}
