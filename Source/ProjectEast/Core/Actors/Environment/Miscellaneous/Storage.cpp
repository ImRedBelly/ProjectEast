﻿#include "Storage.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"

AStorage::AStorage()
{
	InventoryCore = CreateDefaultSubobject<UInventoryCore>(TEXT("InventoryCore"));
}

void AStorage::ClientStartInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		bIsOpen = true;
		InteractableComponent->ToggleInteractionWidget(false);
		OnAnimateOpenChest(true);
		if (auto Controller = Cast<APlayerController>(Interactor))
			Controller->SetIgnoreMoveInput(true);

		if (auto WidgetManager = Cast<AMainPlayerController>(Interactor)->GetWidgetManager())
		 	WidgetManager->SwitchWidgetTo(EWidgetType::Storage);
	}
}

void AStorage::ClientEndInteraction(AActor* Interactor)
{
	if (IsValid(Interactor) && bIsOpen)
	{
		bIsOpen = false;
		InteractableComponent->ToggleInteractionWidget(true);
		OnAnimateOpenChest(false);
		if (auto Controller = Cast<APlayerController>(Interactor))
			Controller->ResetIgnoreMoveInput();

		if (auto WidgetManager = Cast<AMainPlayerController>(Interactor)->GetWidgetManager())
		{
			if (WidgetManager->GetActiveWidget() == EWidgetType::Storage)
				WidgetManager->CloseActiveWidget();
		}
	}
}
