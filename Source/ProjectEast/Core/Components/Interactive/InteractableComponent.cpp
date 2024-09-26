#include "InteractableComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Actors/Interfaces/Interactable.h"
#include "ProjectEast/Core/Actors/Interfaces/ObjectInteraction.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/UI/Misc/Interaction/InteractionWidget.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"


UBoxComponent* UInteractableComponent::GetInteractionVolume() const
{
	return InteractableArea;
}

bool UInteractableComponent::IsInteractable() const
{
	return bIsInteractable;
}

bool UInteractableComponent::IsShowLootBar() const
{
	return bIsShowLootBar;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->Initialize();


	TArray<FName>& OwnerTags = GetOwner()->Tags;
	OwnerTags.AddUnique(InteractableTag);

	if (bDestroyAfterPickup)
		OwnerTags.AddUnique(DestroyableTag);
}

void UInteractableComponent::OnToggleHighlight(bool bIsHighlight, AActor* Interactor)
{
	InteractionWidgetComponent->SetVisibility(bIsHighlight);

	for (int i = 0; i < ObjectsHighlight.Num(); ++i)
	{
		ObjectsHighlight[i]->SetRenderCustomDepth(bIsHighlight);
	}

	SetWidgetLocalOwner(Cast<APlayerController>(Interactor));
}

void UInteractableComponent::SetWidgetLocalOwner(APlayerController* OwningPlayer)
{
	CachedInteractionWidget = CreateWidget<UInteractionWidget>(OwningPlayer, BaseInteractionWidget);
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);

	if (IsValid(CachedInteractionWidget))
	{
		CachedInteractionWidget->SetText(DefaultInteractionText);
		CachedInteractionWidget->SetInputType(InteractionInputType);

		if (IsValid(CachedInteractionWidget))
		{
			InteractionWidgetComponent->SetWidget(CachedInteractionWidget);
			InteractionWidgetComponent->SetOwnerPlayer(CachedInteractionWidget->GetOwningLocalPlayer());
		}
	}
}

void UInteractableComponent::OnPreInteraction(AActor* Interactor, TArray<FEnhancedActionKeyMapping> InteractionKeys)
{
	CachedInteractor = Interactor;
	FindPressedKeyByActionName(InteractionKeys);

	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->ClientPreInteraction(Interactor);

	switch (InteractionInputType)
	{
	case EInteractionInputType::Single:
		if (IObjectInteraction* ObjectInteractable = Cast<IObjectInteraction>(Interactor))
			ObjectInteractable->StartInteractionWithObject(this);
		break;
	case EInteractionInputType::Holding:
		OnDurationPress();
		break;
	case EInteractionInputType::Multiple:
		OnMultiplePress();
		break;
	}
}

void UInteractableComponent::OnDurationPress()
{
	GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
	GetWorld()->GetTimerManager().SetTimer(KeyDownTimer, this, &UInteractableComponent::IsKeyDown,
	                                       InteractionTimerRate, true);
}

void UInteractableComponent::OnMultiplePress()
{
	CurrentClickCount++;
	FillInteractionWidgetBorder(CurrentClickCount * SpeedFillBorder);
	GetWorld()->GetTimerManager().ClearTimer(ClickResetTimerHandle);

	if (CurrentClickCount >= MaxClickCount)
	{
		if (IObjectInteraction* ObjectInteractable = Cast<IObjectInteraction>(CachedInteractor))
			ObjectInteractable->StartInteractionWithObject(this);
		CurrentClickCount = 0;
	}
	else if (CurrentClickCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ClickResetTimerHandle, this,
		                                       &UInteractableComponent::ResetClickCounter, MashingKeyRetriggerableTime,
		                                       true, FirstDelayResetClick);
	}
}


void UInteractableComponent::ResetClickCounter()
{
	if (CurrentClickCount > 0)
	{
		CurrentClickCount--;
		FillInteractionWidgetBorder(CurrentClickCount * SpeedFillBorder);
	}
}

bool UInteractableComponent::CheckForSpecificItemInInventory(AActor* Interactor) const
{
	if (bUseSpecifiedPanel)
	{
		if (!IsValid(Interactor))
			return true;
		
		AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Interactor);
		UInventoryCore* Inventory = PlayerController->GetPlayerInventory();

		if (!IsValid(Inventory) || KeyID.IsEmpty())
			return false;

		auto ItemByID = Inventory->GetItemByID(KeyID, SpecifiedPanel);
		if (ItemByID.Get<1>())
		{
			if (bRemoveItemAfterInteraction)
			{
				Inventory->RemoveItemFromInventoryArray(ItemByID.Get<0>());
			}
			return true;
		}
		PlayerController->GetWidgetManager()->DisplayMessage("Locked. Key Required.");
		return false;
	}
	return true;
}

void UInteractableComponent::StartInteraction()
{
	if (IObjectInteraction* ObjectInteractable = Cast<IObjectInteraction>(CachedInteractor))
		ObjectInteractable->StartInteractionWithObject(this);
}

void UInteractableComponent::Initialize()
{
	IInteractable::Initialize();
}

#pragma region OnInteraction
void UInteractableComponent::Interaction(AActor* Interactor)
{
	if (!CheckForSpecificItemInInventory(Interactor))
		return;

	CachedInteractor = Interactor;
	bIsAlreadyInteracted = true;

	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->Interaction(Interactor);

	OnAssociatedActorInteraction(Interactor);
	RemoveInteractionByResponse();
}

void UInteractableComponent::EndInteraction(AActor* Interactor)
{
}

void UInteractableComponent::RemoveInteraction()
{
}

void UInteractableComponent::GameLoad()
{
}

void UInteractableComponent::ClientStartInteraction(AActor* Interactor)
{
}

void UInteractableComponent::ClientEndInteraction(AActor* Interactor)
{
}

void UInteractableComponent::ClientPreInteraction(AActor* Interactor)
{
}

bool UInteractableComponent::CanBeInteractedWith()
{
	return bIsInteractable;
}

void UInteractableComponent::OnInitialize() const
{
	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->Initialize();
}

void UInteractableComponent::SetupInteractableReferences(UBoxComponent* BoxComponent, UWidgetComponent* WidgetComponent,
                                                         const TArray<UPrimitiveComponent*>& HighlightableObjects)
{
	InteractableArea = BoxComponent;
	InteractionWidgetComponent = WidgetComponent;
	ObjectsHighlight = HighlightableObjects;

	for (UPrimitiveComponent* Component : ObjectsHighlight)
		Component->SetCollisionResponseToChannel(ECC_Interactable, ECR_Block);
}

void UInteractableComponent::OnClientInteraction(AActor* Interactor)
{
	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->ClientStartInteraction(Interactor);
}

void UInteractableComponent::OnAssociatedActorInteraction(AActor* Interactor)
{
	for (auto KV : AssociatedInteractableActors)
	{
		if (IsValid(KV.Key))
		{
			UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
				KV.Key->GetComponentByClass(StaticClass()));

			if (IsValid(InteractableComponent))
				InteractableComponent->OnCheckForInteractionWithAssociate(Interactor);
		}
	}
}

void UInteractableComponent::OnCheckForInteractionWithAssociate(AActor* Interactor)
{
	if (!bIsCheckForAssociatedActors || IsTargetInteractableValue())
	{
		Interaction(Interactor);

		if (bIsRemoveAssociatedInteractablesOnComplete)
		{
			for (auto KV : AssociatedInteractableActors)
			{
				if (IsValid(KV.Key))
				{
					UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
						KV.Key->GetComponentByClass(StaticClass()));
					if (IsValid(InteractableComponent))
						InteractableComponent->OnRemoveInteraction();
				}
			}

			InteractableResponse = EInteractionResponse::OnlyOnce;
		}
		else
		{
			if (InteractableResponse != EInteractionResponse::Persistent)
			{
				for (auto KV : AssociatedInteractableActors)
				{
					if (IsValid(KV.Key))
					{
						UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
							KV.Key->GetComponentByClass(StaticClass()));
						if (IsValid(InteractableComponent))
							InteractableComponent->OnRemoveInteraction();
					}
				}
			}

			switch (InteractableResponse)
			{
			case EInteractionResponse::Persistent:
				break;
			case EInteractionResponse::OnlyOnce:
				InteractableResponse = EInteractionResponse::OnlyOnce;
				break;
			case EInteractionResponse::Temporary:
				ToggleCanBeReInitialized(false);
				break;
			}
		}
	}
}

void UInteractableComponent::RemoveInteractionByResponse()
{
	switch (InteractableResponse)
	{
	case EInteractionResponse::Persistent:
		break;
	case EInteractionResponse::OnlyOnce:
	case EInteractionResponse::Temporary:
		OnRemoveInteraction();
		break;
	}
}

#pragma endregion OnInteraction


#pragma region AfterInteraction
void UInteractableComponent::OnEndInteraction(AActor* Interactor) const
{
	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->EndInteraction(Interactor);
}

void UInteractableComponent::OnClientEndInteraction(AActor* Interactor) const
{
	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->ClientEndInteraction(Interactor);
}

void UInteractableComponent::OnAssociatedActorEndInteraction() const
{
	for (auto KV : AssociatedInteractableActors)
	{
		if (IsValid(KV.Key))
		{
			UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
				KV.Key->GetComponentByClass(StaticClass()));
			if (IsValid(InteractableComponent))
				InteractableComponent->OnEndInteraction(CachedInteractor);
		}
	}
}

void UInteractableComponent::OnRemoveInteraction()
{
	OnClientRemoveInteraction();
}

void UInteractableComponent::OnClientRemoveInteraction()
{
	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->RemoveInteraction();

	bIsInteractable = false;
	InteractableArea = nullptr;

	if (bDestroyAfterPickup)
		GetOwner()->Destroy();
}

void UInteractableComponent::OnReInitialize()
{
	if (bCanReInitialized && InteractableResponse == EInteractionResponse::Temporary)
	{
		if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
			ObjectInteractable->Initialize();

		ToggleIsInteractable(true);

		if (IsValid(CachedInteractionWidget))
			CachedInteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInteractableComponent::OnReInitializeAssociatedActors() const
{
	for (auto KV : AssociatedInteractableActors)
	{
		if (IsValid(KV.Key))
		{
			UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
				KV.Key->GetComponentByClass(StaticClass()));
			if (IsValid(InteractableComponent))
			{
				InteractableComponent->ToggleCanBeReInitialized(true);
				InteractableComponent->OnReInitialize();
			}
		}
	}
}

void UInteractableComponent::OnGameLoaded() const
{
	if (IInteractable* ObjectInteractable = Cast<IInteractable>(GetOwner()))
		ObjectInteractable->GameLoad();

	if (IsValid(CachedInteractionWidget))
		CachedInteractionWidget->SetVisibility(ESlateVisibility::Visible);
}

void UInteractableComponent::ChangeInteractableValue(bool Increment)
{
	if (Increment)
	{
		InteractableValue++;
		if (InteractableValue > InteractableLimitValue)
			InteractableValue = 0;
	}
	else
	{
		InteractableValue--;
		if (InteractableValue < 0)
			InteractableValue = InteractableLimitValue;
	}
}

int UInteractableComponent::GetInteractableValue() const
{
	return InteractableValue;
}

void UInteractableComponent::ToggleInteractionWidget(bool Condition) const
{
	if (bIsInteractable)
	{
		if (IsValid(CachedInteractionWidget))
		{
			CachedInteractionWidget->SetVisibility(Condition ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			CachedInteractionWidget->SetFillDecimalValue(0.05f);
		}
	}
}

#pragma endregion AfterInteraction


bool UInteractableComponent::IsTargetInteractableValue()
{
	bool bIsTargetInteractableValue = false;

	for (auto KV : AssociatedInteractableActors)
	{
		if (IsValid(KV.Key))
		{
			UInteractableComponent* InteractableComponent = Cast<UInteractableComponent>(
				KV.Key->GetComponentByClass(StaticClass()));
			if (KV.Value == InteractableComponent->InteractableValue)
				bIsTargetInteractableValue = true;
			else
			{
				bIsTargetInteractableValue = false;
				break;
			}
		}
	}
	return bIsTargetInteractableValue;
}

void UInteractableComponent::ToggleCanBeReInitialized(bool Condition)
{
	if (Condition != bCanReInitialized)
		bCanReInitialized = Condition;
}

void UInteractableComponent::ToggleIsInteractable(bool Condition)
{
	if (Condition != bIsInteractable)
		bIsInteractable = Condition;
}

void UInteractableComponent::IsKeyDown()
{
	UInteractableComponent* InteractableComponent = InventoryUtility::GetCurrentInteractableObject(CachedInteractor);

	if (IsValid(InteractableComponent) && InteractableComponent == this)
	{
		bool IsCompleted = HoldingInput().Get<0>();
		bool IsReset = HoldingInput().Get<1>();

		if (IsCompleted)
		{
			if (IObjectInteraction* ObjectInteractable = Cast<IObjectInteraction>(CachedInteractor))
			{
				GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
				ObjectInteractable->StartInteractionWithObject(this);
			}
		}
		if (IsReset)
			GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
	}
	else
		GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
}

void UInteractableComponent::FindPressedKeyByActionName(TArray<FEnhancedActionKeyMapping> InteractionKeys)
{
	bool bIsFindKey = false;

	for (const auto& InteractionKey : InteractionKeys)
	{
		if ((InteractionKey.Key.IsGamepadKey() && IconButtonGameModule->IsUsingGamepad()) ||
			(!InteractionKey.Key.IsGamepadKey() && !IconButtonGameModule->IsUsingGamepad()))
		{
			PressedInteractionKey = InteractionKey;
			bIsFindKey = true;
			break;
		}
	}

	if (!bIsFindKey && InteractionKeys.Num() > 0)
	{
		PressedInteractionKey = FEnhancedActionKeyMapping();
		PressedInteractionKey.Key = InteractionKeys[0].Key;
	}
}

void UInteractableComponent::FillInteractionWidgetBorder(float Value) const
{
	if (IsValid(CachedInteractionWidget))
	{
		float RangeValue = UKismetMathLibrary::MapRangeClamped(Value, 0.0f, MaxKeyTimeDown, 0.0f, 1.0f);
		CachedInteractionWidget->OnBorderFill(RangeValue);
	}
}

TTuple<bool, bool> UInteractableComponent::HoldingInput() const
{
	bool IsCompleted = false;
	bool IsReset = false;

	APlayerController* PlayerController = Cast<APlayerController>(CachedInteractor);
	float InputTimeValue = PlayerController->GetInputKeyTimeDown(PressedInteractionKey.Key);

	FillInteractionWidgetBorder(InputTimeValue);

	if (InputTimeValue > MaxKeyTimeDown)
	{
		CachedInteractionWidget->SetFillDecimalValue(0.05f);
		IsCompleted = true;
	}
	else
	{
		if (InputTimeValue <= 0.0f)
			IsReset = true;
	}

	return MakeTuple(IsCompleted, IsReset);
}

//
//
// 	HandleRetriggerableDelay();
// 	return MakeTuple(bCompleted, LocalFloat);
// }

// void UInteractableComponent::HandleRetriggerableDelay()
// {
// 	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UInteractableComponent::HandleRetriggerableDelay,
// 	                                       MashingKeyRetriggerableTime, false);
//
// 	if (LocalFloat >= 0.0f && LocalFloat <= 1.0f)
// 	{
// 		LocalFloat -= Delta;
// 	}
// 	else
// 	{
// 		LocalFloat = 0.0f;
// 		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
// 	}
// }
