#include "Door.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

ADoor::ADoor()
{
	bReplicates = true;
	InteractionWidget->SetupAttachment(StaticMeshComponent);
	InteractableArea->SetupAttachment(StaticMeshComponent);
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bDoorOpen);
}

void ADoor::Interaction(AActor* Interactor)
{
	if (IsValid(Interactor))
		SetOwner(Interactor);

	if (HasAuthority())
	{
		bDoorOpen = !bDoorOpen;
		InteractableComponent->ChangeInteractableValue(true);
		InteractableComponent->RemoveInteraction();

		OnRep_DoorToggle();
	}
}

void ADoor::GameLoad()
{
}

void ADoor::OnDoorRotation(FRotator DoorRotation)
{
	StaticMeshComponent->SetRelativeRotation(DoorRotation);
}

void ADoor::OnRep_DoorToggle()
{
	OnAnimateOpenDoor(bDoorOpen);
}
