#include "Vendor.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"


AVendor::AVendor()
{
	InventoryCore = CreateDefaultSubobject<UInventoryCore>(TEXT("InventoryCore"));
}

void AVendor::ClientStartInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		//GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red, "OPEN VENDOR WIDGET");
		InteractableComponent->ToggleInteractionWidget(false);
		if (auto Controller = Cast<APlayerController>(Interactor))
			Controller->SetIgnoreMoveInput(true);

		if (auto WidgetManager = Cast<AMainPlayerController>(Interactor)->GetWidgetManager())
			WidgetManager->SwitchWidgetTo(EWidgetType::Vendor);
	}
}

void AVendor::ClientEndInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		InteractableComponent->ToggleInteractionWidget(true);
		if (auto Controller = Cast<APlayerController>(Interactor))
			Controller->ResetIgnoreMoveInput();

		if (auto WidgetManager = Cast<AMainPlayerController>(Interactor)->GetWidgetManager())
		{
			if (WidgetManager->GetActiveWidget() == EWidgetType::Vendor)
				WidgetManager->CloseActiveWidget();
		}
	}
}
