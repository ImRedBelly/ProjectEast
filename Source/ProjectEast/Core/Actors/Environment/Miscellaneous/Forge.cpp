#include "Forge.h"

#include "ProjectEast/Core/Actors/Environment/BaseObjects/BaseItemPickUp.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

AForge::AForge(): CraftingCore(nullptr)
{
	CraftingCore = CreateDefaultSubobject<UCraftingCore>(TEXT("CraftingCore"));
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
}

void AForge::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SpawnPoint->SetRelativeLocation(FVector(100, 0, -95));
}

void AForge::ClientStartInteraction(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		InteractableComponent->ToggleInteractionWidget(false);

		if(AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Interactor))
		{
			if(UPlayerCrafting* PlayerCrafting = PlayerController->GetPlayerCrafting())
			{
				PlayerCrafting->OpenCraftingWidget(CraftingCore, EWidgetType::StationCrafting);
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

void AForge::BeginPlay()
{
	Super::BeginPlay();
	CraftingCore->OnSpawnCraftedItem.AddDynamic(this, &AForge::SpawnCraftedItems);
}

void AForge::BeginDestroy()
{
	if (IsValid(CraftingCore))
		CraftingCore->OnSpawnCraftedItem.RemoveDynamic(this, &AForge::SpawnCraftedItems);
	Super::BeginDestroy();
}

void AForge::SpawnCraftedItems(FCraftingData& CraftingData, AActor* ItemOwner)
{
	auto CraftableData = InventoryUtility::GetCraftableData(&CraftingData);
	for (auto ItemData : CraftableData.Get<0>())
	{
		if (InventoryUtility::IsItemClassValid(ItemData))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = ItemOwner;
			SpawnInfo.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;

			auto SpawnClass = ItemData->Class.GetDefaultObject()->InteractableClass;
			FVector Position = SpawnPoint->GetComponentTransform().GetLocation();
			FRotator Rotation = SpawnPoint->GetRelativeRotation();
			auto NewInteractionItem = GetWorld()->SpawnActor<
				ABaseItemPickUp>(SpawnClass, Position, Rotation, SpawnInfo);

			NewInteractionItem->MulticastSetPhysicsSimulation();
			NewInteractionItem->GetInventoryCore()->AddItemToInventoryArray(ItemData, -1);
		}
	}
}
