#include "EquipmentMeshUpdater.h"

#include "PlayerEquipment.h"
#include "GameFramework/Character.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void UEquipmentMeshUpdater::InitializeMeshUpdater(ACharacter* Character, USkeletalMeshComponent* InSkeletalMeshHead,
                                                  USkeletalMeshComponent* InSkeletalMeshGloves,
                                                  USkeletalMeshComponent* InSkeletalMeshChest,
                                                  USkeletalMeshComponent* InSkeletalMeshLegs,
                                                  USkeletalMeshComponent* InSkeletalMeshBoots)
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Character->GetController());
	if (IsValid(PlayerController))
		PlayerEquipment = PlayerController->GetPlayerEquipment();

	if (IsValid(PlayerEquipment))
		PlayerEquipment->OnUpdateEquipmentMesh.AddDynamic(this, &UEquipmentMeshUpdater::UpdateEquipmentMesh);

	SkeletalMeshHead = InSkeletalMeshHead;
	SkeletalMeshGloves = InSkeletalMeshGloves;
	SkeletalMeshChest = InSkeletalMeshChest;
	SkeletalMeshLegs = InSkeletalMeshLegs;
	SkeletalMeshBoots = InSkeletalMeshBoots;
}

void UEquipmentMeshUpdater::BeginDestroy()
{
	if (IsValid(PlayerEquipment))
		PlayerEquipment->OnUpdateEquipmentMesh.RemoveDynamic(this, &UEquipmentMeshUpdater::UpdateEquipmentMesh);
	
	Super::BeginDestroy();
}

void UEquipmentMeshUpdater::UpdateEquipmentMesh(EItemSlot ItemSlot, USkeletalMesh* SkeletalMesh)
{
	if (auto Character = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* RootSkeletalMesh = Character->GetMesh())
		{
			switch (ItemSlot)
			{
			case EItemSlot::Head:
				{
					SkeletalMeshHead->SetSkinnedAssetAndUpdate(SkeletalMesh, false);
					SkeletalMeshHead->SetLeaderPoseComponent(RootSkeletalMesh, false, false);
				}
				break;
			case EItemSlot::Gloves:
				{
					SkeletalMeshGloves->SetSkinnedAssetAndUpdate(SkeletalMesh, false);
					SkeletalMeshGloves->SetLeaderPoseComponent(RootSkeletalMesh, false, false);
				}
				break;
			case EItemSlot::Chest:
				{
					SkeletalMeshChest->SetSkinnedAssetAndUpdate(SkeletalMesh, false);
					SkeletalMeshChest->SetLeaderPoseComponent(RootSkeletalMesh, false, false);
				}
				break;
			case EItemSlot::Legs:
				{
					SkeletalMeshLegs->SetSkinnedAssetAndUpdate(SkeletalMesh, false);
					SkeletalMeshLegs->SetLeaderPoseComponent(RootSkeletalMesh, false, false);
				}
				break;
			case EItemSlot::Boots:
				{
					SkeletalMeshBoots->SetSkinnedAssetAndUpdate(SkeletalMesh, false);
					SkeletalMeshBoots->SetLeaderPoseComponent(RootSkeletalMesh, false, false);
				}
				break;
			default: ;
			}
		}
	}
}
