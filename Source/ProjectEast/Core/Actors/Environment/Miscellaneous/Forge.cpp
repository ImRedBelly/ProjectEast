#include "Forge.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

AForge::AForge(): CraftingCore(nullptr)
{
	CraftingCore = CreateDefaultSubobject<UCraftingCore>(TEXT("CraftingCore"));
}

void AForge::ClientStartInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		InteractableComponent->ToggleInteractionWidget(false);

		if (auto PlayerController = Cast<AMainPlayerController>(Interactor))
		{
			if (UWidgetManager* WidgetManager = PlayerController->GetWidgetManager())
			{
				WidgetManager->InitializeCraftingStation(CraftingCore);
				WidgetManager->OpenNewWidget(EWidgetType::StationCrafting);
			}
		}
	}
}

void AForge::ClientEndInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		InteractableComponent->ToggleInteractionWidget(true);
		if (auto PlayerCrafting = InventoryUtility::GetPlayerCrafting(Interactor))
			PlayerCrafting->CloseCraftingWidget();
	}
}
