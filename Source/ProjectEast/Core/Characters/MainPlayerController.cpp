﻿#include "MainPlayerController.h"

#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEast/Core/Actors/Other/PlayerCapture.h"
#include "ProjectEast/Core/Camera/BasePlayerCameraManager.h"
#include "ProjectEast/Core/Components/PlayerLeveling.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/CharacterSystems/CharacterStatsComponent.h"
#include "ProjectEast/Core/Components/CharacterSystems/ConsumableBuffs.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Components/Equipment/PlayerEquipment.h"
#include "ProjectEast/Core/Components/Interactive/InteractionComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

AMainPlayerController::AMainPlayerController()
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInventory"));
	PlayerEquipment = CreateDefaultSubobject<UPlayerEquipment>(TEXT("PlayerEquipment"));
	ConsumableBuffs = CreateDefaultSubobject<UConsumableBuffs>(TEXT("ConsumableBuffs"));
	CharacterStatsComponent = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("CharacterStatsComponent"));
	PlayerLeveling = CreateDefaultSubobject<UPlayerLeveling>(TEXT("PlayerLeveling"));
	PlayerCrafting = CreateDefaultSubobject<UPlayerCrafting>(TEXT("PlayerCrafting"));
	WidgetManager = CreateDefaultSubobject<UWidgetManager>(TEXT("WidgetManager"));
}

void AMainPlayerController::InitializeInteraction(UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent))
		InteractableComponent->OnPreInteraction(this, InteractionComponent->GetInteractionKeys());
}

void AMainPlayerController::OnInteraction() const
{
	if (!WidgetManager->IsAnyMainWidgetOpen())
		PlayerInventory->InputInteraction();
}

void AMainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitializeComponents();
	InteractionComponent->Activate(true);

	if (auto CameraManager = Cast<ABasePlayerCameraManager>(PlayerCameraManager))
		CameraManager->OnPossess(InPawn);
}

void AMainPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	InteractionComponent->Deactivate();
}

void AMainPlayerController::InputActionInventory()
{
	PlayerInventory->InputInventory();
}

void AMainPlayerController::InitializeComponents()
{
	PlayerInventory->InitializeInventory(this);
	PlayerEquipment->InitializeEquipment();
	PlayerCrafting->InitializeCrafting(this, PlayerInventory);
	InteractionComponent->InitializeInteraction(this);


	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCapture::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		APlayerCapture* PlayerCapture = Cast<APlayerCapture>(FoundActors[0]);
		PlayerCapture->InitializePlayerCapture(GetPlayerEquipment());
		WidgetManager->InitializeWidgetManager(this, PlayerCapture);
	}
}

void AMainPlayerController::InitializeInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent))
		ClientInitializeInteractionWithObject(InteractableComponent);
}

void AMainPlayerController::StartInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->Interaction(this);

		if (IsValid(InteractableComponent->GetOwner()))
		{
			CachedObject = InteractableComponent->GetOwner();
			UInventoryCore* InventoryCore = Cast<UInventoryCore>(
				CachedObject->GetComponentByClass(UInventoryCore::StaticClass()));
			if (IsValid(InventoryCore) && InventoryCore->GetComponentClassCanReplicate())
			{
				PlayerInventory->ServerUpdateItems(CachedObject);
				PlayerInventory->AddViewer(PlayerState, InventoryCore);
			}
		}

		InteractableComponent->OnClientInteraction(this);
	}
}

void AMainPlayerController::EndInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent))
	{
		CachedObject = InteractableComponent->GetOwner();

		UInventoryCore* InventoryCore = Cast<UInventoryCore>(
			CachedObject->GetComponentByClass(UInventoryCore::StaticClass()));
		if (IsValid(InventoryCore))
			PlayerInventory->RemoveViewer(PlayerState, InventoryCore);

		InteractableComponent->OnEndInteraction(this);
		InteractableComponent->OnClientEndInteraction(this);
	}
}

void AMainPlayerController::RemoveInteractionFromObject(UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent) && InteractableComponent->GetOwner()->GetIsReplicated())
		InteractableComponent->OnRemoveInteraction();
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleSideShoulderAction, ETriggerEvent::Completed,
		                                   this, &AMainPlayerController::ToggleSideShoulder);

		EnhancedInputComponent->BindAction(ToggleRagdollAction, ETriggerEvent::Completed,
		                                   this, &AMainPlayerController::ToggleRagdoll);

		EnhancedInputComponent->BindAction(DebugOpenMenuOverlayStatesAction, ETriggerEvent::Started,
		                                   this, &AMainPlayerController::OpenMenuOverlayStates);
		EnhancedInputComponent->BindAction(DebugOpenMenuOverlayStatesAction, ETriggerEvent::Completed,
		                                   this, &AMainPlayerController::CloseMenuOverlayStates);

		EnhancedInputComponent->BindAction(DebugCycleMenuOverlayStatesAction, ETriggerEvent::Started,
		                                   this, &AMainPlayerController::CycleMenuOverlayStatesAction);
	}
}

UPlayerInventory* AMainPlayerController::GetPlayerInventory() const
{
	return PlayerInventory;
}

UPlayerEquipment* AMainPlayerController::GetPlayerEquipment() const
{
	return PlayerEquipment;
}

UConsumableBuffs* AMainPlayerController::GetConsumableBuffs() const
{
	return ConsumableBuffs;
}

UCharacterStatsComponent* AMainPlayerController::GetStatsComponent() const
{
	return CharacterStatsComponent;
}

UPlayerLeveling* AMainPlayerController::GetPlayerLeveling() const
{
	return PlayerLeveling;
}

UWidgetManager* AMainPlayerController::GetWidgetManager() const
{
	return WidgetManager;
}

UPlayerCrafting* AMainPlayerController::GetPlayerCrafting() const
{
	return PlayerCrafting;
}

void AMainPlayerController::ClientInitializeInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	InitializeInteraction(InteractableComponent);
}

void AMainPlayerController::ToggleSideShoulder()
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetCharacter()))
	{
		BaseCharacter->ToggleSideShoulder();
	}
}

void AMainPlayerController::ToggleRagdoll()
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetCharacter()))
	{
		BaseCharacter->ToggleRagdoll();
	}
}

void AMainPlayerController::OpenMenuOverlayStates()
{
	WidgetManager->OpenMenuOverlayStates();
}

void AMainPlayerController::CloseMenuOverlayStates()
{
	WidgetManager->CloseMenuOverlayStates();
}

void AMainPlayerController::CycleMenuOverlayStatesAction(const FInputActionValue& Value)
{
	float UpDirection = Value.Get<float>();
	WidgetManager->CycleStateOverlayStates(UpDirection > 0);
}
