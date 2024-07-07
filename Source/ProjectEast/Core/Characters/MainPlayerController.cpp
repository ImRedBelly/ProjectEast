#include "MainPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractionComponent.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

AMainPlayerController::AMainPlayerController()
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInventory"));
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CachedMainWindow = CreateWidget<UMainWindow>(this, DefaultMainWindow);

	if (UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
		CachedMainWindow->AddToPlayerScreen();
	else
		CachedMainWindow->AddToViewport();
}

void AMainPlayerController::InitializeInteraction(UInteractableComponent* InteractableComponent)
{
	if (IsValid(InteractableComponent))
		InteractableComponent->OnPreInteraction(this);
}

void AMainPlayerController::OnInteraction() const
{
	if (!IsAnyMainWidgetOpen())
		PlayerInventory->InputInteraction();
}

void AMainPlayerController::CloseActiveWidget() const
{
}

bool AMainPlayerController::IsUsingGamepad()
{
	return false;
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
	InventoryUtility::Initialize(GamepadIcons);
	InteractionComponent->InitializeInteraction(this);
	PlayerInventory->InitializeInventory(this);
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

UMainWindow* AMainPlayerController::GetMainWindow() const
{
	return CachedMainWindow;
}

void AMainPlayerController::ClientInitializeInteractionWithObject(UInteractableComponent* InteractableComponent)
{
	InitializeInteraction(InteractableComponent);
}

bool AMainPlayerController::IsAnyMainWidgetOpen() const
{
	return ActiveWidget != EWidgetType::None;
}
