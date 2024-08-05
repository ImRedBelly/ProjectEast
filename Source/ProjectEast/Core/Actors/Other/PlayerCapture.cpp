#include "PlayerCapture.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextRenderComponent.h"
#include "Components/WindDirectionalSourceComponent.h"

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

void APlayerCapture::BeginPlay()
{
	Super::BeginPlay();
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
}

void APlayerCapture::UnbindEventDispatchers()
{
}

void APlayerCapture::StartCapture()
{
	SceneCaptureComponent->bCaptureEveryFrame = true;
	SpawnWindComponent(1.0f,1.0f);
}

void APlayerCapture::EndCapture()
{
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SkeletalMesh->SetRelativeRotation(FRotator::ZeroRotator);
	if(IsValid(WindDirectionalSourceComponent))
	{
		
	}
}

void APlayerCapture::AttachItem()
{
}

void APlayerCapture::DetachItem()
{
}

void APlayerCapture::SpawnWindComponent(float Strength, float Speed)
{
}


void APlayerCapture::AddCaptureRotation(float RotationZ) const
{
	FRotator NewRotation = FRotator(0,RotationZ * -1,0);
	SkeletalMesh->AddLocalRotation(NewRotation, false);
}
