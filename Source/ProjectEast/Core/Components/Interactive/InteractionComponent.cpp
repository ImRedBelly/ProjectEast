#include "InteractionComponent.h"
#include "InteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Utils/GameTypes.h"

class UInteractableComponent;
class UInteractable;

void UInteractionComponent::InitializeInteraction_Implementation(AMainPlayerController* PlayerController)
{
	CachedPlayerController = PlayerController;

	GetWorld()->GetTimerManager().ClearTimer(InteractionTimer);
	GetWorld()->GetTimerManager().SetTimer(InteractionTimer, this, &UInteractionComponent::UpdateInteraction,
										   InteractionTimerRate, true, 1.0f);

	TArray<FEnhancedActionKeyMapping> FoundMappings = MappingContext->GetMappings();
	for (const auto& FoundMapping : FoundMappings)
		if (FoundMapping.Action == InputInteraction)
			InteractionKeys.Add(FoundMapping);
}

UInteractableComponent* UInteractionComponent::GetInteractableComponent() const
{
	return CurrentInteractableComponent;
}

TArray<FEnhancedActionKeyMapping> UInteractionComponent::GetInteractionKeys() const
{
	return InteractionKeys;
}

void UInteractionComponent::UpdateInteraction()
{
	UpdateCurrentInteractable();

	if (IsValid(CachedPlayerController))
	{
		if (IsValid(CachedPlayerController->GetPawn()))
		{
			CachedPawn = CachedPlayerController->GetPawn();

			FVector StartLocation = CachedPawn->GetActorLocation() + FVector(0, 0, 110);

			UCameraComponent* CameraComponent = Cast<UCameraComponent>(CachedPawn->GetComponentByClass(UCameraComponent::StaticClass()));
			if(!IsValid(CameraComponent))
				return;
			
			FVector EndLocation = StartLocation + (CameraComponent->GetForwardVector() * CameraTraceLength);

			FHitResult Result;
			UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation, 35, 0.0f,
			                                         UEngineTypes::ConvertToTraceType(ECC_Interactable), false,
			                                         TArray<AActor*>(), bIsShowDrawTrace? EDrawDebugTrace::ForDuration: EDrawDebugTrace::None,
			                                         Result, true);


			if (Result.bBlockingHit)
			{
				UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
					Result.GetActor()->GetComponentByClass(UInteractableComponent::StaticClass()));

				if (IsValid(InteractableComponent))
				{
					if (InteractableComponent->CanBeInteractedWith())
					{
						auto InteractionVolume = InteractableComponent->GetInteractionVolume();
						if (IsValid(InteractionVolume))
						{
							if (InteractionVolume->IsOverlappingActor(CachedPawn))
							{
								AssignInteractionToLocal(InteractableComponent);
							}
							else
								CheckCurrentInteractableComponent();
						}
						else
							CheckCurrentInteractableComponent();
					}
					else
						CheckCurrentInteractableComponent();
				}
				else
					CheckCurrentInteractableComponent();
			}
			else
			{
				if (!UseOwnerFPTrace())
					CheckCurrentInteractableComponent();
			}
		}
	}
}

void UInteractionComponent::UpdateCurrentInteractable()
{
	if (IsValid(CurrentInteractableComponent))
	{
		if (IInteractable* ObjectInteractable = Cast<IInteractable>(CurrentInteractableComponent->GetOwner()))
			if (!ObjectInteractable->CanBeInteractedWith())
				RemoveInteractionFromCurrent();
	}
}

void UInteractionComponent::RemoveInteractionFromCurrent()
{
	if (IsValid(CurrentInteractableComponent))
	{
		CurrentInteractableComponent->OnToggleHighlight(false, CachedPlayerController);

		if (IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(GetOwner()))
		{
			ObjectInteraction->EndInteractionWithObject(CurrentInteractableComponent);
		}
		CurrentInteractableComponent = nullptr;
	}
}

void UInteractionComponent::AssignInteractionToLocal(UInteractableComponent* InteractableComponent)
{
	if (CurrentInteractableComponent != InteractableComponent)
	{
		RemoveInteractionFromCurrent();
		CurrentInteractableComponent = InteractableComponent;
		CurrentInteractableComponent->OnToggleHighlight(true, CachedPlayerController);
	}
}

bool UInteractionComponent::UseOwnerFPTrace()
{
	FVector StartLocation = CachedPawn->GetActorLocation();

	FVector EndLocation = StartLocation + (CachedPawn->GetActorForwardVector() * OwnerTraceLength);

	FHitResult Result;
	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation, 50.0f, 100.0f,
	                                         UEngineTypes::ConvertToTraceType(ECC_Interactable), false,
	                                         TArray<AActor*>(),
	                                         bIsShowDrawTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	                                         Result, true);

	if (!Result.bBlockingHit)
		return false;


	UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
		Result.GetActor()->GetComponentByClass(UInteractableComponent::StaticClass()));


	if (IsValid(InteractableComponent))
	{
		if (InteractableComponent->CanBeInteractedWith())
		{
			if (IsValid(CurrentInteractableComponent))
			{
				if (IsValid(CurrentInteractableComponent->GetInteractionVolume()))
				{
					if (CurrentInteractableComponent->GetInteractionVolume()->IsOverlappingActor(CachedPawn))
					{
						return true;
					}
					else
					{
						AssignInteractionToLocal(InteractableComponent);
						return true;
					}
				}
				else
				{
					AssignInteractionToLocal(InteractableComponent);
					return true;
				}
			}
			else
			{
				if (IsValid(InteractableComponent->GetInteractionVolume()))
				{
					if (InteractableComponent->GetInteractionVolume()->IsOverlappingActor(CachedPawn))
					{
						AssignInteractionToLocal(InteractableComponent);
						return true;
					}
					return false;
				}
				return false;
			}
		}
		return false;
	}
	return false;
}

void UInteractionComponent::CheckCurrentInteractableComponent()
{
	if (IsValid(CurrentInteractableComponent))
	{
		auto InteractionVolume = CurrentInteractableComponent->GetInteractionVolume();
		if (IsValid(InteractionVolume))
		{
			if (!InteractionVolume->IsOverlappingActor(CachedPawn))
			{
				RemoveInteractionFromCurrent();
			}
		}
		else
			RemoveInteractionFromCurrent();
	}
	else
		UseOwnerFPTrace();
}
