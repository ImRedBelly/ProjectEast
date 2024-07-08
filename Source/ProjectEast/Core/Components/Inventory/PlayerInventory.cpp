#include "PlayerInventory.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Actors/Interactive/ObjectInteraction.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"


void UPlayerInventory::BeginPlay()
{
}

void UPlayerInventory::InitializeInventory(APlayerController* PlayerController)
{
	ClientInitializeInventory(PlayerController);
	Super::InitializeInventory(PlayerController);
}

void UPlayerInventory::ClientInitializeInventory(APlayerController* PlayerController)
{
	CashedPlayerController = Cast<AMainPlayerController>(PlayerController);
	if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		for (auto Panel : PanelsToUse)
			BuildInventory(Panel);
	}
}

void UPlayerInventory::ServerTakeItem(FItemData* ItemData, UInventoryCore* Sender, AActor* OwningPlayer)
{
}

void UPlayerInventory::ServerTakeAllItems(UInventoryCore* Sender, AActor* OwningPlayer)
{
}

void UPlayerInventory::InputCloseWidget()
{
	if(bIsInteractableActorWidgetOpen)
	{
		CashedPlayerController->EndInteractionWithObject(GetCurrentInteractable());
	}
	else
	{
		CashedPlayerController->CloseActiveWidget();
	}
}

void UPlayerInventory::InputInteraction() const
{
	if (bIsLootBarOpen)
	{
		if (OnTakeItem.IsBound())
			OnTakeItem.Broadcast();
	}
	else
	{
		if (!bIsInteractableActorWidgetOpen)
		{
			if (IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(CashedPlayerController))
				ObjectInteraction->InitializeInteractionWithObject(
					InventoryUtility::GetCurrentInteractableObject(CashedPlayerController));
		}
	}
}

UInteractableComponent* UPlayerInventory::GetCurrentInteractable() const
{
	return InventoryUtility::GetCurrentInteractableObject(CashedPlayerController);
}

void UPlayerInventory::OpenLootBarWidget()
{
	InventoryUtility::PlaySoundOnOpeningWidget();
	bIsLootBarOpen = true;
	bIsInteractableActorWidgetOpen = true;
	
	if(IsValid(CashedPlayerController->GetMainWindow()))
	{
		UMainWindow* MainWindow = CashedPlayerController->GetMainWindow();
		MainWindow->ShowLootBar();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(MainWindow->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);
		CashedPlayerController->SetInputMode(InputMode);
		CashedPlayerController->SetShowMouseCursor(true);
		CashedPlayerController->SetIgnoreMoveInput(true);
		CashedPlayerController->SetIgnoreLookInput(true);
		
	}
}

void UPlayerInventory::CloseLootBarWidget()
{
	
	bIsLootBarOpen = false;
	bIsInteractableActorWidgetOpen = false;
	if(IsValid(CashedPlayerController->GetMainWindow()))
	{
		auto MainWindow = CashedPlayerController->GetMainWindow();
		MainWindow->HideLootBar();
		
		FInputModeGameOnly InputMode;
		CashedPlayerController->SetInputMode(InputMode);
		
		CashedPlayerController->SetShowMouseCursor(false);
		CashedPlayerController->ResetIgnoreInputFlags();
		
	}
}
