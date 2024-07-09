#include "BaseInteractable.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

ABaseInteractable::ABaseInteractable()
{
	InteractableArea = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractableArea"));
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractableArea->SetupAttachment(RootComponent);
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	InteractableArea->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
}

void ABaseInteractable::Initialize()
{
	TArray<UPrimitiveComponent*> Array;
	Array.Add(StaticMeshComponent);
	Array.Add(SkeletalMeshComponent);
	InteractableComponent->SetupInteractableReferences(InteractableArea, InteractionWidget, Array);
}

bool ABaseInteractable::CanBeInteractedWith()
{
	return InteractableComponent->IsInteractable();
}
