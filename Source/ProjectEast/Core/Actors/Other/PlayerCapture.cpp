#include "PlayerCapture.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextRenderComponent.h"
#include "Components/WindDirectionalSourceComponent.h"
#include "ProjectEast/Core/Components/Equipment/PlayerEquipment.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

APlayerCapture::APlayerCapture()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	RootCharacter = CreateDefaultSubobject<USceneComponent>("RootCharacter");
	RootCharacter->SetupAttachment(RootComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMeshBoots = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Boots"));
	SkeletalMeshLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	SkeletalMeshGloves = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gloves"));
	SkeletalMeshChest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	SkeletalMeshHead = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hesd"));

	SkeletalMesh->SetupAttachment(RootCharacter);
	SkeletalMeshBoots->SetupAttachment(SkeletalMesh);
	SkeletalMeshLegs->SetupAttachment(SkeletalMesh);
	SkeletalMeshGloves->SetupAttachment(SkeletalMesh);
	SkeletalMeshChest->SetupAttachment(SkeletalMesh);
	SkeletalMeshHead->SetupAttachment(SkeletalMesh);

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponent");
	auto CaptureDirection = CreateDefaultSubobject<UArrowComponent>("CaptureDirection");
	SceneCaptureComponent->SetupAttachment(RootComponent);
	CaptureDirection->SetupAttachment(SceneCaptureComponent);


	WindPosition = CreateDefaultSubobject<UTextRenderComponent>("TextPosition");
	WindPosition->SetupAttachment(RootComponent);
	auto WindDirection = CreateDefaultSubobject<UArrowComponent>("WindDirection");
	WindDirection->SetupAttachment(WindPosition);
}

void APlayerCapture::InitializePlayerCapture(UPlayerEquipment* PlayerEquipment)
{
	if (IsValid(PlayerEquipment))
	{
		CachedPlayerEquipment = PlayerEquipment;
		BindEventDispatchers();
	}
}

void APlayerCapture::BindEventDispatchers()
{
	CachedPlayerEquipment->OnItemAttach.AddDynamic(this, &APlayerCapture::AttachItem);
	CachedPlayerEquipment->OnItemDetach.AddDynamic(this, &APlayerCapture::DetachItem);
}

void APlayerCapture::UnbindEventDispatchers()
{
	if (IsValid(CachedPlayerEquipment))
		CachedPlayerEquipment->OnItemAttach.RemoveDynamic(this, &APlayerCapture::AttachItem);
	CachedPlayerEquipment->OnItemDetach.RemoveDynamic(this, &APlayerCapture::DetachItem);
}

void APlayerCapture::StartCapture()
{
	SceneCaptureComponent->bCaptureEveryFrame = true;
	SpawnWindComponent(1.0f, 1.0f);
}

void APlayerCapture::EndCapture()
{
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SkeletalMesh->SetRelativeRotation(FRotator::ZeroRotator);
	if (IsValid(WindDirectionalSourceComponent))
	{
	}
}

void APlayerCapture::AttachItem(FItemData& ItemData)
{
	if (InventoryUtility::IsItemClassValid(&ItemData))
	{
		auto SkeletalMeshAttach = ItemData.Class.GetDefaultObject()->SkeletalMesh;
		switch (ItemData.EquipmentSlot)
		{
		case EItemSlot::Head:
			{
				SkeletalMeshHead->SetSkinnedAssetAndUpdate(SkeletalMeshAttach, true);
				SkeletalMeshHead->SetLeaderPoseComponent(SkeletalMesh, true, false);
			}
			break;
		case EItemSlot::Gloves:
			{
				SkeletalMeshGloves->SetSkinnedAssetAndUpdate(SkeletalMeshAttach, true);
				SkeletalMeshGloves->SetLeaderPoseComponent(SkeletalMesh, true, false);
			}
			break;
		case EItemSlot::Chest:
			{
				SkeletalMeshChest->SetSkinnedAssetAndUpdate(SkeletalMeshAttach, true);
				SkeletalMeshChest->SetLeaderPoseComponent(SkeletalMesh, true, false);
			}
			break;
		case EItemSlot::Legs:
			{
				SkeletalMeshLegs->SetSkinnedAssetAndUpdate(SkeletalMeshAttach, true);
				SkeletalMeshLegs->SetLeaderPoseComponent(SkeletalMesh, true, false);
			}
			break;
		case EItemSlot::Boots:
			{
				SkeletalMeshBoots->SetSkinnedAssetAndUpdate(SkeletalMeshAttach, true);
				SkeletalMeshBoots->SetLeaderPoseComponent(SkeletalMesh, true, false);
			}
			break;
		default: ;
		}
	}
	PrestreamTextures(0, true, 0);
}

void APlayerCapture::DetachItem(FItemData& ItemData)
{
		switch (ItemData.EquipmentSlot)
		{
		case EItemSlot::Head:
			{
				SkeletalMeshHead->SetSkinnedAssetAndUpdate(nullptr, true);
			}
			break;
		case EItemSlot::Gloves:
			{
				SkeletalMeshGloves->SetSkinnedAssetAndUpdate(nullptr, true);
			}
			break;
		case EItemSlot::Chest:
			{
				SkeletalMeshChest->SetSkinnedAssetAndUpdate(nullptr, true);
			}
			break;
		case EItemSlot::Legs:
			{
				SkeletalMeshLegs->SetSkinnedAssetAndUpdate(nullptr, true);
			}
			break;
		case EItemSlot::Boots:
			{
				SkeletalMeshBoots->SetSkinnedAssetAndUpdate(nullptr, true);
			}
			break;
		default: ;
		}
}

void APlayerCapture::SpawnWindComponent(float Strength, float Speed)
{
}


void APlayerCapture::AddCaptureRotation(float RotationZ) const
{
	FRotator NewRotation = FRotator(0, RotationZ * -1, 0);
	SkeletalMesh->AddLocalRotation(NewRotation, false);
}
