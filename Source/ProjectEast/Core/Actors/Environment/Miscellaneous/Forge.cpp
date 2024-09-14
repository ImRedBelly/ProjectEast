#include "Forge.h"

#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

AForge::AForge(): CraftingCore(nullptr)
{
	CraftingCore = CreateDefaultSubobject<UCraftingCore>(TEXT("CraftingCore"));
}

void AForge::ClientStartInteraction(AActor* Interactor)
{
	if(IsValid(Interactor))
	{
		InteractableComponent->ToggleInteractionWidget(false);
		if(auto PlayerCrafting = InventoryUtility::GetPlayerCrafting(Interactor))
			PlayerCrafting->OpenCraftingWidget(CraftingCore, EWidgetType::StationCrafting);
	}
}

void AForge::ClientEndInteraction(AActor* Interactor)
{
	if(IsValid(Interactor))
	{
		InteractableComponent->ToggleInteractionWidget(true);
		if(auto PlayerCrafting = InventoryUtility::GetPlayerCrafting(Interactor))
			PlayerCrafting->CloseCraftingWidget();
	}
}
