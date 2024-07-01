#include "MainPlayerController.h"
#include "ProjectEast/Core/Components/Interactive/InteractionComponent.h"

AMainPlayerController::AMainPlayerController()
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

bool AMainPlayerController::IsUsingGamepad()
{
	return true;
}


void AMainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitializeComponents();
	InteractionComponent->Activate(true);
}

void AMainPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	InteractionComponent->Deactivate();
}

void AMainPlayerController::InitializeComponents()
{
	InteractionComponent->InitializeInteraction(this);
}

void AMainPlayerController::InitializeInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	
}

void AMainPlayerController::StartInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	
}

void AMainPlayerController::EndInteractionWithObject(UInteractableComponent* InteractableComponent)
{

}

void AMainPlayerController::RemoveInteractionFromObject(UInteractableComponent* InteractableComponent)
{
	
}
