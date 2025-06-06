﻿#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "MainPlayerController.h"
#include "Animations/BaseCharacterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/CharacterSystems/ConsumableBuffs.h"
#include "ProjectEast/Core/Components/Debug/DebugComponent.h"
#include "ProjectEast/Core/Components/Movement/BaseCharacterMovementComponent.h"
#include "ProjectEast/Core/Components/Movement/WallRunComponent.h"
#include "ProjectEast/Core/Library/Structurs/Character/GetUpAnimation.h"

class UEnhancedInputLocalPlayerSubsystem;


const FName NAME_Pelvis(TEXT("Pelvis"));
const FName NAME_RagdollPose(TEXT("RagdollPose"));
const FName NAME_root(TEXT("root"));

FVector ABaseCharacter::GetMovementInput() const
{
	return ReplicatedCurrentAcceleration;
}

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = 0;
	bReplicates = true;
	SetReplicatingMovement(true);

	WallRunComponent = CreateDefaultSubobject<UWallRunComponent>(TEXT("WallRunComponent"));
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BaseCharacterMovementComponent = Cast<UBaseCharacterMovementComponent>(Super::GetMovementComponent());
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->AddTickPrerequisiteActor(this);
	SetMovementModel();
	ForceUpdateCharacterState();
	MainAnimInstance = GetMesh()->GetAnimInstance();
	TargetRotation = GetActorRotation();
	LastVelocityRotation = TargetRotation;
	LastMovementInputRotation = TargetRotation;

	if (MainAnimInstance && GetLocalRole() == ROLE_SimulatedProxy)
	{
		MainAnimInstance->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	BaseCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	DebugComponent = FindComponentByClass<UDebugComponent>();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetEssentialValues(DeltaTime);
	DrawDebugShapes();

	switch (MovementState)
	{
	case EMovementState::Grounded:
		{
			UpdateCharacterMovement();
			UpdateGroundedRotation(DeltaTime);
		}
		break;
	case EMovementState::InAir:
		{
			UpdateInAirRotation();
			if (bHasMovementInput)
				MantleCheck(FallingTraceSettings, EDrawDebugTrace::Type::ForDuration);
		}
		break;
	case EMovementState::Ragdoll:
		{
			RagdollUpdate(DeltaTime);
			break;
		}
	default: ;
	}

	PreviousVelocity = GetVelocity();
	PreviousAimYaw = AimingRotation.Yaw;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(ABaseCharacter, TargetRagdollLocation);
	// DOREPLIFETIME_CONDITION(ABaseCharacter, ReplicatedCurrentAcceleration, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(ABaseCharacter, ReplicatedControlRotation, COND_SkipOwner);
	//
	// DOREPLIFETIME(ABaseCharacter, DesiredGait);
	// DOREPLIFETIME_CONDITION(ABaseCharacter, DesiredStance, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(ABaseCharacter, DesiredRotationMode, COND_SkipOwner);
	//
	// DOREPLIFETIME_CONDITION(ABaseCharacter, RotationMode, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(ABaseCharacter, OverlayState, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(ABaseCharacter, ViewMode, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ABaseCharacter, VisibleMesh, COND_SkipOwner);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IsValid(MappingContext))
				Subsystem->RemoveMappingContext(MappingContext);
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}


	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
		                                   this, &ABaseCharacter::PlayerMovementInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed,
		                                   this, &ABaseCharacter::PlayerMovementInput);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
		                                   this, &ABaseCharacter::PlayerLookInput);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
		                                   this, &ABaseCharacter::PlayerStartJumpInput);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
		                                   this, &ACharacter::StopJumping);

		// Walk
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed,
		                                   this, &ABaseCharacter::PlayerWalkInput);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,
		                                   this, &ABaseCharacter::PlayerStartSprintInput);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed,
		                                   this, &ABaseCharacter::PlayerStopSprintInput);
		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed,
		                                   this, &ABaseCharacter::PlayerCrouchInput);
		// AimAction
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started,
		                                   this, &ABaseCharacter::PlayerStartAimInput);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed,
		                                   this, &ABaseCharacter::PlayerStopAimInput);

		//Interaction
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this,
		                                   &ABaseCharacter::OnInteractive);
		EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this,
		                                   &ABaseCharacter::OnOpenInventory);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this,
		                                   &ABaseCharacter::OnPause);

		EnhancedInputComponent->BindAction(UsePocket1Action, ETriggerEvent::Started, this,
		                                   &ABaseCharacter::UsePocket1);
		EnhancedInputComponent->BindAction(UsePocket2Action, ETriggerEvent::Started, this,
		                                   &ABaseCharacter::UsePocket2);
		EnhancedInputComponent->BindAction(UsePocket3Action, ETriggerEvent::Started, this,
		                                   &ABaseCharacter::UsePocket3);
		EnhancedInputComponent->BindAction(UsePocket4Action, ETriggerEvent::Started, this,
		                                   &ABaseCharacter::UsePocket4);
	}
}

#pragma region Input
void ABaseCharacter::PlayerMovementInput(const FInputActionValue& Value)
{
	switch (MovementState)
	{
	case EMovementState::Grounded:
	case EMovementState::InAir:
		{
			FVector2D MovementVector = Value.Get<FVector2D>();


			auto ControlSideVector = GetControlForwardRightVector();
			auto DiagonalValues = FixDiagonalGamepadValues(MovementVector.Y, MovementVector.X);

			LastPlayerMovementInput = (DiagonalValues.Get<0>() * ControlSideVector.Get<0>() +
				DiagonalValues.Get<1>() * ControlSideVector.Get<1>());
			LastPlayerMovementInput.Normalize(.0001f);

			AddMovementInput(ControlSideVector.Get<0>(), DiagonalValues.Get<0>());
			AddMovementInput(ControlSideVector.Get<1>(), DiagonalValues.Get<1>());
		}
		break;
	default: ;
	}
}

void ABaseCharacter::PlayerLookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y * LookUpDownRate);
	AddControllerYawInput(LookAxisVector.X * LookLeftRightRate);
}

void ABaseCharacter::PlayerWalkInput(const FInputActionValue& Value)
{
	switch (DesiredGait)
	{
	case EGait::Walking:
		DesiredGait = EGait::Running;
		break;
	case EGait::Running:
		DesiredGait = EGait::Walking;
		break;
	default: ;
	}
}

void ABaseCharacter::PlayerStartSprintInput(const FInputActionValue& Value)
{
	DesiredGait = EGait::Sprinting;
}

void ABaseCharacter::PlayerStopSprintInput(const FInputActionValue& Value)
{
	DesiredGait = EGait::Running;
}

void ABaseCharacter::PlayerCrouchInput(const FInputActionValue& Value)
{
	if (MovementAction == EMovementAction::None)
	{
		CheckMultiTapInputCrouch();
		if (!IsMultiTapInputCrouch)
		{
			if (MovementState == EMovementState::Grounded)
			{
				if (Stance == EStance::Standing)
				{
					DesiredStance = EStance::Crouching;
					Crouch();
				}
				else if (Stance == EStance::Crouching)
				{
					DesiredStance = EStance::Standing;
					UnCrouch();
				}
			}
			else if (MovementState == EMovementState::InAir)
				SetBreakFall();
		}
		else
		{
			OnPlayRollAnimation();
			if (Stance == EStance::Standing)
				DesiredStance = EStance::Crouching;
			else if (Stance == EStance::Crouching)
				DesiredStance = EStance::Standing;
		}
	}
}

void ABaseCharacter::PlayerStartJumpInput(const FInputActionValue& Value)
{
	if (WallRunComponent->StartWallJump())
	{
	}
	else
	{
		Jump();
	}
}

void ABaseCharacter::PlayerStartAimInput(const FInputActionValue& Value)
{
	SetRotationMode(ERotationMode::Aiming);
}

void ABaseCharacter::PlayerStopAimInput(const FInputActionValue& Value)
{
	if (ViewMode == EViewMode::ThirdPerson)
		SetRotationMode(DesiredRotationMode);
	else if (ViewMode == EViewMode::FirstPerson)
		SetRotationMode(ERotationMode::LookingDirection);
}

void ABaseCharacter::OnInteractive()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->OnInteraction();
}

void ABaseCharacter::OnOpenInventory()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetWidgetManager()->OpenNewWidget(EWidgetType::Inventory);
}

void ABaseCharacter::OnPause()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetWidgetManager()->OpenNewWidget(EWidgetType::Pause);
}

void ABaseCharacter::UsePocket1()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(0);
}

void ABaseCharacter::UsePocket2()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(1);
}

void ABaseCharacter::UsePocket3()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(2);
}

void ABaseCharacter::UsePocket4()
{
	if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(Controller))
		PlayerController->GetConsumableBuffs()->OnUsePocket(3);
}

#pragma endregion Input

void ABaseCharacter::Jump()
{
	if (MovementAction == EMovementAction::None)
	{
		switch (MovementState)
		{
		case EMovementState::Grounded:
			{
				if (bHasMovementInput && MantleCheck(GroundedTraceSettings, EDrawDebugTrace::Type::ForDuration))
					return;

				switch (Stance)
				{
				case EStance::Standing:
					Super::Jump();
					break;
				case EStance::Crouching:
					{
						UnCrouch();
					}
					break;
				}
			}
			break;
		case EMovementState::InAir:
			MantleCheck(FallingTraceSettings, EDrawDebugTrace::Type::ForDuration);
			break;
		case EMovementState::Ragdoll:
			//TODO RagdollEnd
			break;
		default: ;
		}
	}
}

void ABaseCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	InAirRotation = Speed > 100 ? LastVelocityRotation : GetActorRotation();
	if (IsValid(MainAnimInstance))
		Cast<UBaseCharacterAnimInstance>(MainAnimInstance)->OnJumped();
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (GetCharacterMovement()->MovementMode == MOVE_Walking || GetCharacterMovement()->MovementMode == MOVE_NavWalking)
	{
		SetMovementState(EMovementState::Grounded);
	}
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		SetMovementState(EMovementState::InAir);
	}
}

void ABaseCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OnStanceChanged(EStance::Crouching);
}

void ABaseCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OnStanceChanged(EStance::Standing);
}

void ABaseCharacter::OnBeginPlay()
{
}

void ABaseCharacter::ForceUpdateCharacterState()
{
	SetGait(DesiredGait, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
	SetViewMode(ViewMode, true);
	SetOverlayState(OverlayState, true);
	SetMovementState(MovementState, true);
	SetMovementAction(MovementAction, true);
}

void ABaseCharacter::SetMovementModel()
{
	MovementData = MovementModel.DataTable->FindRow<FMovementSettingsState>(MovementModel.RowName,
	                                                                        TEXT(""));
}

void ABaseCharacter::UpdateCharacterMovement()
{
	AllowedGait = GetAllowedGait();
	ActualGait = GetActualGait(AllowedGait);

	SetGait(ActualGait);
	UpdateDynamicMovementSettings(AllowedGait);
}

void ABaseCharacter::UpdateDynamicMovementSettings(EGait InAllowedGait)
{
	CurrentMovementSettings = GetTargetMovementSettings();

	float FutureSpeed = 0.0f;

	switch (InAllowedGait)
	{
	case EGait::Walking:
		FutureSpeed = CurrentMovementSettings.WalkSpeed;
		break;
	case EGait::Running:
		FutureSpeed = CurrentMovementSettings.RunSpeed;
		break;
	case EGait::Sprinting:
		FutureSpeed = CurrentMovementSettings.SprintSpeed;
		break;
	}

	FVector VectorValue = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed());

	GetCharacterMovement()->MaxWalkSpeed = FutureSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = FutureSpeed;
	GetCharacterMovement()->MaxAcceleration = VectorValue.X;
	GetCharacterMovement()->BrakingDecelerationWalking = VectorValue.Y;
	GetCharacterMovement()->GroundFriction = VectorValue.Z;
}

FMovementSettings ABaseCharacter::GetTargetMovementSettings() const
{
	if (RotationMode == ERotationMode::VelocityDirection)
	{
		if (Stance == EStance::Standing)
			return MovementData->VelocityDirection.Standing;
		if (Stance == EStance::Crouching)
			return MovementData->VelocityDirection.Crouching;
	}
	else if (RotationMode == ERotationMode::LookingDirection)
	{
		if (Stance == EStance::Standing)
			return MovementData->LookingDirection.Standing;
		if (Stance == EStance::Crouching)
			return MovementData->LookingDirection.Crouching;
	}
	else if (RotationMode == ERotationMode::Aiming)
	{
		if (Stance == EStance::Standing)
			return MovementData->Aiming.Standing;
		if (Stance == EStance::Crouching)
			return MovementData->Aiming.Crouching;
	}

	return MovementData->VelocityDirection.Standing;
}

void ABaseCharacter::OnMovementStateChanged(EMovementState NewMovementState)
{
	PreviousMovementState = MovementState;
	MovementState = NewMovementState;
	if (MovementState == EMovementState::InAir)
	{
		if (MovementAction == EMovementAction::None)
		{
			InAirRotation = GetActorRotation();
			if (Stance == EStance::Crouching)
			{
				UnCrouch();
			}
		}
		//if (MovementAction == EMovementAction::Rolling)
		//RagdollStart();
	}
	if (MovementState == EMovementState::Ragdoll)
	{
		//if(PreviousMovementState == EMovementState::Mantling)
		//MantleTimeline->Stop();
	}
}

void ABaseCharacter::OnMovementActionChanged(EMovementAction NewMovementAction)
{
	PreviousMovementAction = MovementAction;
	MovementAction = NewMovementAction;

	if (MovementAction == EMovementAction::Rolling)
	{
		Crouch();
	}

	if (PreviousMovementAction == EMovementAction::Rolling)
	{
		if (DesiredStance == EStance::Standing)
		{
			UnCrouch();
		}
		else if (DesiredStance == EStance::Crouching)
		{
			Crouch();
		}
	}
}

void ABaseCharacter::OnRotationModeChanged(ERotationMode NewRotationMode)
{
	PreviousRotationMode = RotationMode;
	RotationMode = NewRotationMode;

	if (RotationMode == ERotationMode::VelocityDirection && ViewMode == EViewMode::FirstPerson)
		SetViewMode(EViewMode::ThirdPerson);
}


void ABaseCharacter::OnGaitChanged(EGait NewGait)
{
	PreviousActualGait = Gait;
	Gait = NewGait;
}

void ABaseCharacter::OnViewModeChanged(EViewMode NewViewMode)
{
	PreviousViewMode = ViewMode;
	ViewMode = NewViewMode;

	if (ViewMode == EViewMode::ThirdPerson)
	{
		if (RotationMode == ERotationMode::VelocityDirection || RotationMode == ERotationMode::LookingDirection)
			SetRotationMode(DesiredRotationMode);
	}
	if (ViewMode == EViewMode::FirstPerson)
	{
		if (RotationMode == ERotationMode::VelocityDirection)
			SetRotationMode(ERotationMode::LookingDirection);
	}
}

void ABaseCharacter::OnOverlayStateChanged(EOverlayState NewOverlayState)
{
	PreviousOverlayState = OverlayState;
	OverlayState = NewOverlayState;
}

void ABaseCharacter::OnStanceChanged(EStance NewStance)
{
	PreviousStance = Stance;
	Stance = NewStance;
}

void ABaseCharacter::SetOverlayOverrideState(int32 NewState)
{
	OverlayOverrideState = NewState;
}

void ABaseCharacter::SetGroundedEntryState(EGroundedEntryState NewState)
{
	GroundedEntryState = NewState;
}

EGait ABaseCharacter::GetAllowedGait() const
{
	if (Stance == EStance::Crouching || (Stance == EStance::Standing && RotationMode == ERotationMode::Aiming))
	{
		if (DesiredGait == EGait::Running || DesiredGait == EGait::Sprinting)
			return EGait::Running;
		if (DesiredGait == EGait::Walking)
			return EGait::Walking;
	}
	if (Stance == EStance::Standing)
	{
		if (RotationMode == ERotationMode::VelocityDirection || RotationMode == ERotationMode::LookingDirection)
		{
			switch (DesiredGait)
			{
			case EGait::Walking:
				return EGait::Walking;
			case EGait::Running:
				return EGait::Running;
			case EGait::Sprinting:
				return CanSprint() ? EGait::Sprinting : EGait::Running;
			}
		}
	}
	return EGait::Walking;
}

EGait ABaseCharacter::GetActualGait(EGait InAllowedGait)
{
	float DeltaSpeed = 10;
	LocalWalkSpeed = CurrentMovementSettings.WalkSpeed;
	LocalRunSpeed = CurrentMovementSettings.RunSpeed;
	LocalSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed >= LocalRunSpeed + DeltaSpeed)
	{
		if (InAllowedGait == EGait::Walking || InAllowedGait == EGait::Running)
			return EGait::Running;
		if (InAllowedGait == EGait::Sprinting)
			return EGait::Sprinting;
	}
	else
	{
		if (Speed >= LocalWalkSpeed + DeltaSpeed)
			return EGait::Running;
		return EGait::Walking;
	}
	return EGait::Walking;
}

bool ABaseCharacter::CanSprint() const
{
	if (bHasMovementInput)
	{
		const float MinMovementInputThreshold = 0.9f;
		switch (RotationMode)
		{
		case ERotationMode::VelocityDirection:
			return MovementInputAmount > MinMovementInputThreshold;
		case ERotationMode::LookingDirection:
			{
				const float MaxYawDifferenceThreshold = 50.0f;

				const FRotator AccelerationRotation = ReplicatedCurrentAcceleration.ToOrientationRotator();
				FRotator ControlRotation = GetControlRotation();
				FRotator DeltaRotation = AccelerationRotation - ControlRotation;
				DeltaRotation.Normalize();

				float AbsYawDifference = FMath::Abs(DeltaRotation.Yaw);
				return (MovementInputAmount > MinMovementInputThreshold) &&
					(AbsYawDifference < MaxYawDifferenceThreshold);
			}
		case ERotationMode::Aiming:
			return false;
		}
	}
	return false;
}

float ABaseCharacter::GetMappedSpeed() const
{
	const float LocWalkSpeed = CurrentMovementSettings.WalkSpeed;
	const float LocRunSpeed = CurrentMovementSettings.RunSpeed;
	const float LocSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed > LocRunSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({LocRunSpeed, LocSprintSpeed}, {2.0f, 3.0f}, Speed);
	}

	if (Speed > LocWalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({LocWalkSpeed, LocRunSpeed}, {1.0f, 2.0f}, Speed);
	}

	return FMath::GetMappedRangeValueClamped<float, float>({0.0f, LocWalkSpeed}, {0.0f, 1.0f}, Speed);
}

void ABaseCharacter::OnPlayRollAnimation()
{
	if (IsValid(MainAnimInstance))
	{
		MainAnimInstance->Montage_Play(GetRollAnimation(), 1.15f);
	}
}

UAnimMontage* ABaseCharacter::GetRollAnimation()
{
	return RollAnimMontage;
}

#pragma region SetNewStates
void ABaseCharacter::SetMovementState(const EMovementState NewState, bool bForce)
{
	if (bForce || MovementState != NewState)
		OnMovementStateChanged(NewState);
}

void ABaseCharacter::SetMovementAction(const EMovementAction NewAction, bool bForce)
{
	if (bForce || MovementAction != NewAction)
		OnMovementActionChanged(NewAction);
}

void ABaseCharacter::SetRotationMode(const ERotationMode NewRotation, bool bForce)
{
	if (bForce || RotationMode != NewRotation)
		OnRotationModeChanged(NewRotation);
}

void ABaseCharacter::SetStance(const EStance NewStance, bool bForce)
{
	if (bForce || Stance != NewStance)
		OnStanceChanged(NewStance);
}

void ABaseCharacter::SetGait(const EGait NewGait, bool bForce)
{
	if (bForce || Gait != NewGait)
		OnGaitChanged(NewGait);
}

void ABaseCharacter::SetViewMode(const EViewMode NewViewMode, bool bForce)
{
	if (bForce || ViewMode != NewViewMode)
		OnViewModeChanged(NewViewMode);
}

void ABaseCharacter::SetOverlayState(const EOverlayState NewOverlayState, bool bForce)
{
	if (bForce || OverlayState != NewOverlayState)
		OnOverlayStateChanged(NewOverlayState);
}

void ABaseCharacter::UpdateGroundedRotation(float DeltaTime)
{
	if (MovementAction == EMovementAction::None)
	{
		if (CanUpdateMovingRotation())
		{
			const float GroundedRotationRate = CalculateGroundedRotationRate();
			if (RotationMode == ERotationMode::VelocityDirection)
			{
				SmoothCharacterRotation({0.0f, LastVelocityRotation.Yaw, 0.0f}, 800.0f, GroundedRotationRate);
			}
			else if (RotationMode == ERotationMode::LookingDirection)
			{
				float YawValue;
				if (Gait == EGait::Sprinting)
				{
					YawValue = LastVelocityRotation.Yaw;
				}
				else
				{
					const float YawOffsetCurveVal = GetAnimCurveValue("YawOffset");
					YawValue = AimingRotation.Yaw + YawOffsetCurveVal;
				}
				SmoothCharacterRotation({0.0f, YawValue, 0.0f}, 500.0f, GroundedRotationRate);
			}
			else if (RotationMode == ERotationMode::Aiming)
			{
				const float ControlYaw = AimingRotation.Yaw;
				SmoothCharacterRotation({0.0f, ControlYaw, 0.0f}, 1000.0f, 20.0f);
			}
		}
		else
		{
			if ((ViewMode == EViewMode::ThirdPerson && RotationMode == ERotationMode::Aiming) ||
				ViewMode == EViewMode::FirstPerson)
			{
				LimitRotation(-100.0f, 100.0f, 20.0f);
			}

			const float RotAmountCurve = GetAnimCurveValue("RotationAmount");

			if (FMath::Abs(RotAmountCurve) > 0.001f)
			{
				if (GetLocalRole() == ROLE_AutonomousProxy)
				{
					TargetRotation.Yaw = UKismetMathLibrary::NormalizeAxis(
						TargetRotation.Yaw + (RotAmountCurve * (DeltaTime / (1.0f / 30.0f))));
					SetActorRotation(TargetRotation);
				}
				else
				{
					AddActorWorldRotation({0, RotAmountCurve * (DeltaTime / (1.0f / 30.0f)), 0});
				}
				TargetRotation = GetActorRotation();
			}
		}
	}
	else if (MovementAction == EMovementAction::Rolling)
	{
		if (bHasMovementInput)
		{
			SmoothCharacterRotation({0.0f, LastMovementInputRotation.Yaw, 0.0f}, 0.0f, 2.0f);
		}
	}
}

void ABaseCharacter::UpdateValueAirRotation()
{
	InAirRotation = GetActorRotation();
}

void ABaseCharacter::UpdateInAirRotation()
{
	switch (RotationMode)
	{
	case ERotationMode::VelocityDirection:
	case ERotationMode::LookingDirection:
		SmoothCharacterRotation(FRotator(0, InAirRotation.Yaw, 0), 0, 5);
		break;
	case ERotationMode::Aiming:
		{
			SmoothCharacterRotation(FRotator(0, GetControlRotation().Yaw, 0), 0, 15);
			InAirRotation = GetActorRotation();
		}
		break;
	}
}

void ABaseCharacter::SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed)
{
	float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	TargetRotation = FMath::RInterpConstantTo(TargetRotation, Target, DeltaSeconds, TargetInterpSpeed);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, ActorInterpSpeed));
}

void ABaseCharacter::AddToCharacterRotation(FRotator DeltaRotation)
{
	TargetRotation += DeltaRotation;
	AddActorWorldRotation(DeltaRotation);
}

void ABaseCharacter::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed)
{
	FRotator Delta = AimingRotation - GetActorRotation();
	Delta.Normalize();
	const float RangeVal = Delta.Yaw;

	if (RangeVal < AimYawMin || RangeVal > AimYawMax)
	{
		const float ControlRotYaw = AimingRotation.Yaw;
		const float TargetYaw = ControlRotYaw + (RangeVal > 0.0f ? AimYawMin : AimYawMax);
		SmoothCharacterRotation({0.0f, TargetYaw, 0.0f}, 0.0f, InterpSpeed);
	}
}

TTuple<FHitResult*, bool> ABaseCharacter::SetActorLocationAndTargetRotation(
	FVector NewLocation, FRotator NewRotation, bool bSweep, bool bTeleport)
{
	FHitResult* Result = nullptr;
	TargetRotation = NewRotation;
	auto isSetActor = SetActorLocationAndRotation(NewLocation, TargetRotation, bSweep, Result,
	                                              bTeleport ? ETeleportType::ResetPhysics : ETeleportType::None);
	return MakeTuple(Result, isSetActor);
}


float ABaseCharacter::CalculateGroundedRotationRate() const
{
	const float MappedSpeedVal = GetMappedSpeed();
	const float CurveVal = CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped<float, float>(
		{0.0f, 300.0f}, {1.0f, 3.0f}, AimYawRate);
	return CurveVal * ClampedAimYawRate;
}

bool ABaseCharacter::CanUpdateMovingRotation() const
{
	return ((bIsMoving && bHasMovementInput) || Speed > 150.0f) && !HasAnyRootMotion();
	//&&(!WallRunComponent->GetIsWallRunning() && !WallRunComponent->GetIsWallJumping());
}

#pragma endregion SetNewStates

bool ABaseCharacter::MantleCheck(FMantleTraceSettings TraceSettings, EDrawDebugTrace::Type DebugTrace)
{
	auto OffsetPlayerInput = (LastPlayerMovementInput * -30.0f);
	auto StartOffsetZ = (TraceSettings.MinLedgeHeight + TraceSettings.MaxLedgeHeight) * 0.5f;
	FVector StartLocation = GetCapsuleBaseLocation(2.0f) + OffsetPlayerInput + FVector(0, 0, StartOffsetZ);

	FVector EndLocation = StartLocation + (LastPlayerMovementInput * TraceSettings.ReachDistance);

	auto CapsuleHalfHeight = ((TraceSettings.MaxLedgeHeight - TraceSettings.MinLedgeHeight) * 0.5f) + 1;

	FCollisionQueryParams TraceParams(FName(TEXT("CapsuleTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult HitResult;


	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Climbable,
		FCollisionShape::MakeCapsule(TraceSettings.ForwardTraceRadius, CapsuleHalfHeight),
		TraceParams);

	// if (DebugTrace == EDrawDebugTrace::Type::ForDuration ||
	// 	DebugTrace == EDrawDebugTrace::Type::ForOneFrame)
	// {
	// 	DrawDebugCapsule(
	// 		GetWorld(),
	// 		(StartLocation + EndLocation) * 0.5f,
	// 		CapsuleHalfHeight,
	// 		TraceSettings.ForwardTraceRadius,
	// 		FQuat::Identity,
	// 		bHit ? FColor::Green : FColor::Red,
	// 		false,
	// 		5.0f);
	// }

	bool IsWalkable = GetCharacterMovement()->IsWalkable(HitResult);

	if (!IsWalkable && HitResult.bBlockingHit && !HitResult.bStartPenetrating)
	{
		FVector InitialTraceImpactPoint = HitResult.ImpactPoint;
		FVector InitialTraceNormal = HitResult.ImpactNormal;
		return true;
	}

	return false;
}

UAnimMontage* ABaseCharacter::GetGetUpAnimation(bool bRagdollFaceUpState) const
{
	if (IsValid(FrontGetUpAnimations) && IsValid(BackGetUpAnimations))
	{
		UDataTable* DataTable = (bRagdollFaceUpState ? BackGetUpAnimations : FrontGetUpAnimations);

		for (auto RowName : DataTable->GetRowNames())
		{
			FGetUpAnimation* GetUpAnimation = DataTable->FindRow<FGetUpAnimation>(RowName, "");
			if (GetUpAnimation->OverlayState == OverlayState)
				return GetUpAnimation->GetUpMontage;
		}
	}

	return nullptr;
}

void ABaseCharacter::RagdollStart()
{
	// Step 1: Clear the Character Movement Mode and set the Movement State to Ragdoll
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetMovementState(EMovementState::Ragdoll);

	// Step 2: Disable capsule collision and enable mesh physics simulation starting from the pelvis.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_Pelvis, true, true);

	// Step 3: Stop any active montages.
	if (IsValid(MainAnimInstance))
	{
		MainAnimInstance->Montage_Stop(0.2f);
	}

	// Fixes character mesh is showing default A pose for a split-second just before ragdoll ends in listen server games
	GetMesh()->bOnlyAllowAutonomousTickPose = true;

	SetReplicateMovement(false);
}

void ABaseCharacter::RagdollEnd()
{
	if (IsValid(MainAnimInstance))
		MainAnimInstance->SavePoseSnapshot(NAME_RagdollPose);

	if (bRagdollOnGround)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		if (IsValid(MainAnimInstance))
		{
			MainAnimInstance->Montage_Play(GetGetUpAnimation(bRagdollFaceUp), 1.0f,
			                               EMontagePlayReturnType::MontageLength, 0.0f, true);
		}
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}

void ABaseCharacter::RagdollUpdate(float DeltaTime)
{
	GetMesh()->bOnlyAllowAutonomousTickPose = false;

	// Set the Last Ragdoll Velocity.
	const FVector NewRagdollVel = GetMesh()->GetPhysicsLinearVelocity(NAME_root);
	LastRagdollVelocity = (NewRagdollVel != FVector::ZeroVector || IsLocallyControlled())
		                      ? NewRagdollVel
		                      : LastRagdollVelocity / 2;

	// Use the Ragdoll Velocity to scale the ragdoll's joint strength for physical animation.
	const float SpringValue = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 1000.0f}, {0.0f, 25000.0f},
	                                                                          LastRagdollVelocity.Size());
	GetMesh()->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);

	// Disable Gravity if falling faster than -4000 to prevent continual acceleration.
	// This also prevents the ragdoll from going through the floor.
	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	GetMesh()->SetEnableGravity(bEnableGrav);

	// Update the Actor location to follow the ragdoll.
	SetActorLocationDuringRagdoll(DeltaTime);
}


void ABaseCharacter::SetActorLocationDuringRagdoll(float DeltaTime)
{
	if (IsLocallyControlled())
	{
		// Set the pelvis as the target location.
		TargetRagdollLocation = GetMesh()->GetSocketLocation(NAME_Pelvis);
		if (!HasAuthority())
		{
			Server_SetMeshLocationDuringRagdoll(TargetRagdollLocation);
		}
	}

	// Determine whether the ragdoll is facing up or down and set the target rotation accordingly.
	const FRotator PelvisRot = GetMesh()->GetSocketRotation(NAME_Pelvis);

	if (bReversedPelvis)
	{
		bRagdollFaceUp = PelvisRot.Roll > 0.0f;
	}
	else
	{
		bRagdollFaceUp = PelvisRot.Roll < 0.0f;
	}


	const FRotator TargetRagdollRotation(0.0f, bRagdollFaceUp ? PelvisRot.Yaw - 180.0f : PelvisRot.Yaw, 0.0f);

	// Trace downward from the target location to offset the target location,
	// preventing the lower half of the capsule from going through the floor when the ragdoll is laying on the ground.
	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
	                        TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect,
	                                                  ECC_Visibility, Params);

	// if (ALSDebugComponent && ALSDebugComponent->GetShowTraces())
	// {
	// 	UALSDebugComponent::DrawDebugLineTraceSingle(World,
	// 	                                             TargetRagdollLocation,
	// 	                                             TraceVect,
	// 	                                             EDrawDebugTrace::Type::ForOneFrame,
	// 	                                             bHit,
	// 	                                             HitResult,
	// 	                                             FLinearColor::Red,
	// 	                                             FLinearColor::Green,
	// 	                                             1.0f);
	// }

	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;

	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}
	// if (!IsLocallyControlled())
	// {
	// 	ServerRagdollPull = FMath::FInterpTo(ServerRagdollPull, 750.0f, DeltaTime, 0.6f);
	// 	float RagdollSpeed = FVector(LastRagdollVelocity.X, LastRagdollVelocity.Y, 0).Size();
	// 	FName RagdollSocketPullName = RagdollSpeed > 300 ? NAME_spine_03 : NAME_pelvis;
	// 	GetMesh()->AddForce(
	// 		(TargetRagdollLocation - GetMesh()->GetSocketLocation(RagdollSocketPullName)) * ServerRagdollPull,
	// 		RagdollSocketPullName, true);
	// }
	SetActorLocationAndTargetRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation, TargetRagdollRotation);
}

void ABaseCharacter::Server_SetMeshLocationDuringRagdoll_Implementation(FVector MeshLocation)
{
	TargetRagdollLocation = MeshLocation;
}

FCameraViewParameters ABaseCharacter::GetCameraParameters()
{
	return FCameraViewParameters(ThirdPersonFOV, FirstPersonFOV, RightShoulder);
}

FVector ABaseCharacter::GetFPCameraTarget()
{
	//return GetMesh()->GetSocketLocation(SocketFPCamera);
	return GetMesh()->GetSocketLocation("FP_Camera");
}

FTransform ABaseCharacter::GetTPPivotTarget()
{
	TArray<FVector> AverageVector;
	AverageVector.Add(GetMesh()->GetSocketLocation("Head"));
	AverageVector.Add(GetMesh()->GetSocketLocation("root"));

	return FTransform(GetActorRotation(), UKismetMathLibrary::GetVectorArrayAverage(AverageVector));
}

FTPTraceParams ABaseCharacter::GetTPTraceParams()
{
	return FTPTraceParams(
		GetMesh()->GetSocketLocation(RightShoulder ? "TP_CameraTrace_R" : "TP_CameraTrace_L"),
		15,
		ECC_Camera);
}

void ABaseCharacter::ToggleSideShoulder()
{
	RightShoulder = !RightShoulder;
}

void ABaseCharacter::ToggleRagdoll()
{
	if (MovementState != EMovementState::Ragdoll)
		RagdollStart();
	else
		RagdollEnd();
}

FCharacterStates ABaseCharacter::GetCurrentStates()
{
	CharacterStates.PawnMovementMode = GetCharacterMovement()->MovementMode;
	CharacterStates.MovementState = MovementState;
	CharacterStates.PrevMovementState = PreviousMovementState;
	CharacterStates.MovementAction = MovementAction;
	CharacterStates.RotationMode = RotationMode;
	CharacterStates.ActualGait = Gait;
	CharacterStates.ActualStance = Stance;
	CharacterStates.ViewMode = ViewMode;
	CharacterStates.OverlayState = OverlayState;
	return CharacterStates;
}

FEssentialValues ABaseCharacter::GetEssentialValues()
{
	EssentialValues.Velocity = GetVelocity();
	EssentialValues.Acceleration = Acceleration;
	EssentialValues.MovementInput = GetCharacterMovement()->GetCurrentAcceleration();
	EssentialValues.IsMoving = bIsMoving;
	EssentialValues.HasMovementInput = bHasMovementInput;
	EssentialValues.Speed = Speed;
	EssentialValues.MovementInputAmount = MovementInputAmount;
	EssentialValues.AimingRotation = GetControlRotation();
	EssentialValues.AimYawRate = AimYawRate;
	return EssentialValues;
}

void ABaseCharacter::SetEssentialValues(float DeltaTime)
{
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		ReplicatedCurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
		ReplicatedControlRotation = GetControlRotation();
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration() != 0
			                       ? GetCharacterMovement()->GetMaxAcceleration()
			                       : EasedMaxAcceleration / 2;
	}


	AimingRotation = FMath::RInterpTo(AimingRotation, ReplicatedControlRotation, DeltaTime, 30);

	const FVector CurrentVel = GetVelocity();

	const FVector NewAcceleration = (CurrentVel - PreviousVelocity) / DeltaTime;
	Acceleration = NewAcceleration.IsNearlyZero() || IsLocallyControlled() ? NewAcceleration : Acceleration / 2;

	Speed = CurrentVel.Size2D();
	bIsMoving = Speed > 1.0f;
	if (bIsMoving)
	{
		LastVelocityRotation = CurrentVel.ToOrientationRotator();
	}

	MovementInputAmount = ReplicatedCurrentAcceleration.Size() / EasedMaxAcceleration;
	bHasMovementInput = MovementInputAmount > 0.0f;
	if (bHasMovementInput)
	{
		LastMovementInputRotation = ReplicatedCurrentAcceleration.ToOrientationRotator();
	}
	AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime);
}

TTuple<FVector, FVector> ABaseCharacter::GetControlForwardRightVector() const
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	return MakeTuple(ForwardDirection, RightDirection);
}

FVector ABaseCharacter::GetCapsuleBaseLocation(float ZOffset) const
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight() + ZOffset;
	FVector CapsuleUpVector = Capsule->GetUpVector() * CapsuleHalfHeight;
	return Capsule->GetComponentLocation() - CapsuleUpVector;
}

float ABaseCharacter::GetAnimCurveValue(FName CurveName) const
{
	if (IsValid(MainAnimInstance))
	{
		return MainAnimInstance->GetCurveValue(CurveName);
	}
	return 0.0f;
}

// FVector ABaseCharacter::GetCapsuleBaseLocation(float ZOffset) const
// {
// }
//
// FVector ABaseCharacter::GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset) const
// {
// }
//
// float ABaseCharacter::GetAnimCurveValue(FName CurveName) const
// {
// }

TTuple<float, float> ABaseCharacter::FixDiagonalGamepadValues(float Y, float X)
{
	float MappedX = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f), FVector2D(1.0f, 1.2f), FMath::Abs(X));
	float MappedY = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f), FVector2D(1.0f, 1.2f), FMath::Abs(Y));

	float NewY = FMath::Clamp(Y * MappedX, -1.0f, 1.0f);
	float NewX = FMath::Clamp(X * MappedY, -1.0f, 1.0f);

	return MakeTuple(NewY, NewX);
}

void ABaseCharacter::DrawDebugShapes()
{
}

EDrawDebugTrace::Type ABaseCharacter::GetTraceDebugType(EDrawDebugTrace::Type DebugTrace)
{
	return DebugTrace;
}
