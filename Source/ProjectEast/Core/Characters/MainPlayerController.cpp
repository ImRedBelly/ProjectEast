#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
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
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CachedMainWindow = CreateWidget<UMainWindow>(this, DefaultMainWindow);

	if (UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
		CachedMainWindow->AddToPlayerScreen();
	else
		CachedMainWindow->AddToViewport();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCapture::StaticClass(), FoundActors);

	if(FoundActors.Num() > 0)
		CachedPlayerCapture = Cast<APlayerCapture>(FoundActors[0]);
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

void AMainPlayerController::CloseActiveWidget()
{
	switch (ActiveWidget)
	{
	case EWidgetType::Inventory:
		PlayerInventory->CloseInventoryWidget();
		break;
	case EWidgetType::Storage:
		PlayerInventory->CloseStorageWidget();
		break;
	default: ;
	}
}

bool AMainPlayerController::IsUsingGamepad()
{
	return false;
}

void AMainPlayerController::OpenNewWidget(EWidgetType WidgetType)
{
	switch (WidgetType)
	{
	case EWidgetType::Inventory:
		PlayerInventory->OpenInventoryWidget();
		break;
	case EWidgetType::Storage:
		PlayerInventory->OpenStorageWidget();
		break;
	default: ;
	}
}

void AMainPlayerController::SetActiveWidget(EWidgetType WidgetType)
{
	ActiveWidget = WidgetType;
}

EWidgetType AMainPlayerController::GetActiveWidget()
{
	return ActiveWidget;
}

EWidgetType AMainPlayerController::GetActivePopup()
{
	return EWidgetType::Inventory;
}

void AMainPlayerController::SwitchWidgetTo(EWidgetType WidgetType)
{
	if(WidgetType != ActiveWidget)
	{
		CloseActiveWidget();
		OpenNewWidget(WidgetType);
		ActiveWidget = WidgetType;
	}
}

void AMainPlayerController::StartPlayerCapture()
{
	CachedPlayerCapture->StartCapture();
}

void AMainPlayerController::StopPlayerCapture()
{
	CachedPlayerCapture->EndCapture();
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
			UInventoryCore* InventoryCore = Cast<UInventoryCore>(CachedObject->GetComponentByClass(UInventoryCore::StaticClass()));
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

APlayerCapture* AMainPlayerController::GetPlayerCapture() const
{
	return CachedPlayerCapture;
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
