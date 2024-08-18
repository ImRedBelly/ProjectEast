#include "MainPlayerController.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/Components/Interactive/InteractionComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

AMainPlayerController::AMainPlayerController()
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInventory"));
	PlayerEquipment = CreateDefaultSubobject<UPlayerEquipment>(TEXT("PlayerEquipment"));
	ConsumableBuffs = CreateDefaultSubobject<UConsumableBuffs>(TEXT("ConsumableBuffs"));
	WidgetManager = CreateDefaultSubobject<UWidgetManager>(TEXT("WidgetManager"));
}

void AMainPlayerController::InitializeInteraction(UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent))
		InteractableComponent->OnPreInteraction(this);
}

void AMainPlayerController::OnInteraction() const
{
	if (!WidgetManager->IsAnyMainWidgetOpen())
		PlayerInventory->InputInteraction();
}


bool AMainPlayerController::IsUsingGamepad()
{
	return false;
}


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	WidgetManager->InitializeWidgetManager(this, PlayerInventory);
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

void AMainPlayerController::InputActionInventory()
{
	PlayerInventory->InputInventory();
}

void AMainPlayerController::InitializeComponents()
{
	//InventoryUtility::Initialize(GamepadIcons);
	InteractionComponent->InitializeInteraction(this);
	PlayerInventory->InitializeInventory(this);
	PlayerEquipment->InitializeEquipment();
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

UWidgetManager* AMainPlayerController::GetWidgetManager() const
{
	return WidgetManager;
}


void AMainPlayerController::ClientInitializeInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	InitializeInteraction(InteractableComponent);
}
