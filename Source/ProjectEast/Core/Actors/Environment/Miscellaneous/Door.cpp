#include "Door.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

ADoor::ADoor()
{
	InteractionWidget->SetupAttachment(StaticMeshComponent);
	InteractableArea->SetupAttachment(StaticMeshComponent);
}

void ADoor::Interaction(AActor* Interactor)
{
	if (IsValid(Interactor))
		SetOwner(Interactor);

	OnUnlock();
}

void ADoor::GameLoad()
{
	FRotator CurrentRotation = InteractableComponent->GetInteractableValue() == 0
		                           ? FRotator(0, 0, 0)
		                           : FRotator(0, 0, 90);
	StaticMeshComponent->SetRelativeRotation(CurrentRotation);
}

void ADoor::OnUnlock()
{
	InteractableComponent->ChangeInteractableValue(true);

	FRotator CurrentRotation = StaticMeshComponent->GetRelativeRotation();
	CurrentRotation.Yaw = bIsDoorOpen ? 0.0f : RotationAngle;
	StaticMeshComponent->SetRelativeRotation(CurrentRotation);

	bIsDoorOpen = !bIsDoorOpen;
}
