#include "PlayerCharacter.h"
#include "MainPlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectEast/Core/Components/Movement/WallRunComponent.h"
#include "ProjectEast/Core/Components/Movement/PlayerMovementComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovementComponent>(CharacterMovementComponentName))
{
	PlayerMovementComponent = StaticCast<UPlayerMovementComponent*>(GetCharacterMovement());

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PlayerMovementComponent->bOrientRotationToMovement = true;
	PlayerMovementComponent->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	PlayerMovementComponent->SetCrouchedHalfHeight(60.f);
	PlayerMovementComponent->JumpZVelocity = 700.f;
	PlayerMovementComponent->AirControl = 0.35f;
	PlayerMovementComponent->MaxWalkSpeed = 270.0f;
	PlayerMovementComponent->MinAnalogWalkSpeed = 20.f;
	PlayerMovementComponent->BrakingDecelerationWalking = 2000.f;
	PlayerMovementComponent->BrakingDecelerationFalling = 1500.0f;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	WallRunComponent = CreateDefaultSubobject<UWallRunComponent>(TEXT("WallRun"));
}

FRotator APlayerCharacter::GetAimOffset() const
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);

	FRotator Result = AimDirectionLocal.ToOrientationRotator();
	return Result;
}

void APlayerCharacter::SetStateStrafe(bool state) const
{
	PlayerMovementComponent->bUseControllerDesiredRotation = state;
	PlayerMovementComponent->bOrientRotationToMovement = !state;
}

void APlayerCharacter::SetGeneralMappingContext()
{
	SetMappingContext(GeneralMappingContext);
}

void APlayerCharacter::SetUIMappingContext()
{
	SetMappingContext(UIMappingContext);
}

void APlayerCharacter::SetMappingContext(UInputMappingContext* MappingContext)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IsValid(CurrentMappingContext))
				Subsystem->RemoveMappingContext(CurrentMappingContext);
			CurrentMappingContext = MappingContext;
			Subsystem->AddMappingContext(CurrentMappingContext, 0);
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetGeneralMappingContext();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateIKSettings(DeltaSeconds);
	TryChangeSprintState();
	TryChangeTargetArmLength();
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed,this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,this, &APlayerCharacter::Look);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,this, &ACharacter::StopJumping);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,this, &APlayerCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed,this, &APlayerCharacter::StopSprint);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started,this, &APlayerCharacter::ChangeCrouchState);
		//EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed,this, &APlayerCharacter::ChangeCrouchState);

		// Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started,this, &APlayerCharacter::OnStartAiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed,this, &APlayerCharacter::OnEndAiming);
		// SlidingAction
		EnhancedInputComponent->BindAction(SlidingAction, ETriggerEvent::Started, this,&APlayerCharacter::OnStartSliding);
		// EnhancedInputComponent->BindAction(SlidingAction, ETriggerEvent::Completed,
		//                                    this, &APlayerCharacter::OnEndSliding);

		//Interaction
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this,&APlayerCharacter::OnInteractive);
		EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this,&APlayerCharacter::OnOpenInventory);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this,&APlayerCharacter::OnPause);
		
		EnhancedInputComponent->BindAction(UsePocket1Action, ETriggerEvent::Started, this,&APlayerCharacter::UsePocket1);
		EnhancedInputComponent->BindAction(UsePocket2Action, ETriggerEvent::Started, this,&APlayerCharacter::UsePocket2);
		EnhancedInputComponent->BindAction(UsePocket3Action, ETriggerEvent::Started, this,&APlayerCharacter::UsePocket3);
		EnhancedInputComponent->BindAction(UsePocket4Action, ETriggerEvent::Started, this,&APlayerCharacter::UsePocket4);

		
		//
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (bIsSliding && MovementVector == FVector2D::Zero())
		OnEndSliding();
	else if (bIsSliding)
		MovementVector = FVector2D::UnitY();


	if (MovementVector == FVector2D::Zero() && bIsSprintRequested && PlayerMovementComponent->IsSprinting())
		StopSprint(0);


	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Jump()
{
	if (PlayerMovementComponent->IsCrouching() && !CheckHeightForUncrouch())
	{
		bIsCrouchRequested = false;
		SetTargetArmLength();
		UnCrouch();
	}
	else if (PlayerMovementComponent->IsSliding())
	{
		OnEndSliding();
	}
	else if (WallRunComponent->StartWallJump())
	{
	}
	else
	{
		Super::Jump();
	}
}

void APlayerCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f
		&& !PlayerMovementComponent->IsSprinting() && !CheckHeightForUncrouch())
	{
		if (PlayerMovementComponent->IsCrouching())
		{
			bIsCrouchRequested = false;
			UnCrouch();
		}

		//SetStateAiming(false);
		bIsSprintRequested = true;
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f &&
		PlayerMovementComponent->IsSprinting()) // && !bIsSliding)
	{
		//TODOCHECK SPRINT BUTTON IN END SLIDE

		bIsSprintRequested = false;
	}
}

void APlayerCharacter::TryChangeSprintState()
{
	if (bIsSprintRequested && !PlayerMovementComponent->IsFalling()
		&& !PlayerMovementComponent->IsSprinting() && !bIsSliding)
	{
		PlayerMovementComponent->StartSprint();
		SetTargetArmLength();
	}
	if (!bIsSprintRequested && PlayerMovementComponent->IsSprinting()) // && !bIsSliding)
	{
		PlayerMovementComponent->StopSprint();
		SetTargetArmLength();
	}
}


void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetTargetOffset(true, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetTargetOffset(false, HalfHeightAdjust);
}

void APlayerCharacter::ChangeCrouchState()
{
	if (!PlayerMovementComponent->IsCrouching() && !PlayerMovementComponent->IsSprinting() && !bIsSliding)
	{
		Crouch();
		bIsCrouchRequested = true;
		SetTargetArmLength();
	}
	else if (PlayerMovementComponent->IsCrouching() && !CheckHeightForUncrouch() && !bIsSliding)
	{
		UnCrouch();
		bIsCrouchRequested = false;
		SetTargetArmLength();
	}
}

bool APlayerCharacter::CheckHeightForUncrouch() const
{
	FVector ActorLocation = GetActorLocation();
	FVector TraceStart(ActorLocation.X, ActorLocation.Y, 0);
	FVector TraceEnd = TraceStart + FVector::UpVector * HeightCheckUncrouch;

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1, 0, 1);

	FHitResult HitResult;
	return GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);
}

void APlayerCharacter::TryChangeTargetArmLength()
{
	if (bIsChangeTargetArmLength)
	{
		bIsChangeTargetArmLength = FMathf::Abs(SpringArmComponent->TargetArmLength - CurrentTargetArmLength) > 0.1f;
		if (!bIsChangeTargetArmLength)
			return;

		float ValueArmLength = FMathf::Lerp(SpringArmComponent->TargetArmLength, CurrentTargetArmLength, 0.075f);
		SpringArmComponent->TargetArmLength = ValueArmLength;
	}
}

void APlayerCharacter::SetTargetArmLength()
{
	bIsChangeTargetArmLength = true;
	CurrentTargetArmLength = GetTargetArm();
}

void APlayerCharacter::SetTargetOffset(bool IsAppend, float NewTargetOffsetZ) const
{
	if (IsAppend)
		SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, NewTargetOffsetZ);
	else
		SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, NewTargetOffsetZ);
}

void APlayerCharacter::OnStartAiming(const FInputActionValue& Value)
{
	SetStateAiming(true);
}

void APlayerCharacter::OnEndAiming(const FInputActionValue& Value)
{
	SetStateAiming(false);
}

void APlayerCharacter::SetStateAiming(bool IsAiming)
{
	if (bIsAiming == IsAiming)
		return;

	bIsAiming = IsAiming;
	SetStateStrafe(bIsAiming);
	SetTargetArmLength();
}

void APlayerCharacter::OnStartSliding()
{
	if (!PlayerMovementComponent->IsSprinting() || bIsSliding)
		return;

	SetTargetOffset(true, TargetOffsetHalfHeightInSliding);
	bIsSliding = true;
	SetTargetArmLength();

	GetWorld()->GetTimerManager().ClearTimer(SlidingTimer);
	GetWorld()->GetTimerManager().SetTimer(SlidingTimer, this, &APlayerCharacter::OnEndSliding, 1, false);

	PlayerMovementComponent->StartSliding();
}

void APlayerCharacter::OnEndSliding()
{
	if (!bIsSliding)
		return;

	SetTargetOffset(false, TargetOffsetHalfHeightInSliding);
	bIsSliding = false;
	PlayerMovementComponent->StopSliding();
	GetWorld()->GetTimerManager().ClearTimer(SlidingTimer);


	if (bIsSprintRequested && !CheckHeightForUncrouch())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
		bIsCrouchRequested = true;
	}


	SetTargetArmLength();
}

void APlayerCharacter::OnInteractive()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->OnInteraction();
}

void APlayerCharacter::OnOpenInventory()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetWidgetManager()->OpenNewWidget(EWidgetType::Inventory);
}

void APlayerCharacter::OnPause()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetWidgetManager()->OpenNewWidget(EWidgetType::Pause);
}

void APlayerCharacter::UsePocket1()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(0);
}
void APlayerCharacter::UsePocket2()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(1);
}
void APlayerCharacter::UsePocket3()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(2);
}
void APlayerCharacter::UsePocket4()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(3);
}

void APlayerCharacter::UpdateIKSettings(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, CalculateIKParametersForSocketName(RightFootSocketName),
	                                     DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, CalculateIKParametersForSocketName(LeftFootSocketName),
	                                    DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, CalculateIKPelvisOffset(), DeltaSeconds, IKInterpSpeed);
}

float APlayerCharacter::CalculateIKPelvisOffset() const
{
	return IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

float APlayerCharacter::CalculateIKParametersForSocketName(const FName& SocketName) const
{
	float Result = 0.0f;

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	const FVector TraceEnd = TraceStart - (CapsuleHalfHeight + IKTraceDistance) * FVector::UpVector;

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);


	const FVector FootSizeBox = FVector(1.f, .15f, 7.f);
	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceEnd, FootSizeBox,
	                                         GetMesh()->GetSocketRotation(SocketName), TraceType, true,
	                                         TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true,
	                                         FLinearColor::Red, FLinearColor::Blue, 1))
	{
		float CharacterBottom = TraceStart.Z - CapsuleHalfHeight;
		Result = CharacterBottom - HitResult.Location.Z;
	}

	return Result;
}

float APlayerCharacter::GetTargetArm() const
{
	float Result = NormalCameraTargetArmLength;

	if (PlayerMovementComponent->IsSprinting() || bIsSliding)
		Result = SprintCameraTargetArmLength;
	else if (bIsAiming)
		Result = AimCameraTargetArmLength;
	else if (bIsCrouchRequested)
		Result = CrouchCameraTargetArmLength;


	return Result;
}
