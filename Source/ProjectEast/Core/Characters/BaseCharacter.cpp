#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Animations/BaseCharacterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEast/Core/Components/Debug/DebugComponent.h"
#include "ProjectEast/Core/Components/Movement/BaseCharacterMovementComponent.h"

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
			UpdateGroundedRotation();
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
			RagdollUpdate();
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
		                                   this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
		                                   this, &ACharacter::StopJumping);

		// Walk
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started,
		                                   this, &ABaseCharacter::PlayerWalkInput);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,
		                                   this, &ABaseCharacter::PlayerStartSprintInput);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed,
		                                   this, &ABaseCharacter::PlayerStopSprintInput);
		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started,
		                                   this, &ABaseCharacter::PlayerCrouchInput);
		// AimAction
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started,
		                                   this, &ABaseCharacter::PlayerStartAimInput);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed,
		                                   this, &ABaseCharacter::PlayerStopAimInput);
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
				if (Stance == EStance::Crouching)
				{
					DesiredStance = EStance::Standing;
					UnCrouch();
				}
			}
			if (MovementState == EMovementState::InAir)
				SetBreakFall();
		}
		if (IsMultiTapInputCrouch)
		{
			//OnPlayRollAnimation();
			if (Stance == EStance::Standing)
				DesiredStance = EStance::Crouching;
			if (Stance == EStance::Crouching)
				DesiredStance = EStance::Standing;
		}
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
	if (ViewMode == EViewMode::FirstPerson)
		SetRotationMode(ERotationMode::LookingDirection);
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
					UnCrouch();
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
	InAirRotation = Speed > 100 ? GetActorRotation() : LastVelocityRotation;
	if(IsValid(MainAnimInstance))
		Cast<UBaseCharacterAnimInstance>(MainAnimInstance)->OnJumped();

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "OnJumped");
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
				UnCrouch();
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
		Crouch();

	if (PreviousMovementAction == EMovementAction::Rolling)
	{
		if (DesiredStance == EStance::Standing)
			UnCrouch();
		if (DesiredStance == EStance::Crouching)
			Crouch();
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
	float LocWalkSpeed = CurrentMovementSettings.WalkSpeed;
	float LocRunSpeed = CurrentMovementSettings.RunSpeed;
	float LocSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed > LocRunSpeed)
		return FMath::GetMappedRangeValueClamped<float, float>({LocRunSpeed, LocSprintSpeed}, {2.0f, 3.0f}, Speed);
	if (Speed > LocWalkSpeed)
		return FMath::GetMappedRangeValueClamped<float, float>({LocWalkSpeed, LocRunSpeed}, {1.0f, 2.0f}, Speed);

	return FMath::GetMappedRangeValueClamped<float, float>({0.0f, LocWalkSpeed}, {0.0f, 1.0f}, Speed);
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

void ABaseCharacter::UpdateGroundedRotation()
{
	switch (MovementAction)
	{
	case EMovementAction::None:
		{
			if (CanUpdateMovingRotation())
			{
				switch (RotationMode)
				{
				case ERotationMode::VelocityDirection:
					SmoothCharacterRotation(FRotator(0, LastVelocityRotation.Yaw, 0), 800,
					                        CalculateGroundedRotationRate());
					break;
				case ERotationMode::LookingDirection:
					{
						switch (Gait)
						{
						case EGait::Walking:
						case EGait::Running:
							{
								SmoothCharacterRotation(
									FRotator(0, GetControlRotation().Yaw + GetAnimCurveValue("YawOffset"), 0), 500,
									CalculateGroundedRotationRate());
							}
							break;
						case EGait::Sprinting:
							SmoothCharacterRotation(FRotator(0, LastVelocityRotation.Yaw, 0), 500,
							                        CalculateGroundedRotationRate());
							break;
						}
					}
					break;
				case ERotationMode::Aiming:
					SmoothCharacterRotation(FRotator(0, GetControlRotation().Yaw, 0), 1000, 20);
					break;
				}
			}
			else
			{
				if (ViewMode == EViewMode::FirstPerson || (ViewMode == EViewMode::ThirdPerson && RotationMode ==
					ERotationMode::Aiming))
				{
					LimitRotation(-100, 100, 20);
				}
				float RotationAmount = GetAnimCurveValue("RotationAmount");
				if (FMath::Abs(RotationAmount) > 0.001f)
				{
					AddActorWorldRotation(FRotator(0, RotationAmount * GetWorld()->GetDeltaSeconds() / (1.0f / 30.0f),
					                               0));
					TargetRotation = GetActorRotation();
				}
			}
		}
		break;
	case EMovementAction::Rolling:
		{
			if (bHasMovementInput)
			{
				SmoothCharacterRotation(FRotator(0, LastMovementInputRotation.Yaw, 0), 0, 2);
			}
		}
		break;
	}
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
	FRotator ControlRotation = GetControlRotation();
	FRotator ActorRotation = GetActorRotation();

	FRotator DeltaRotation = ControlRotation - ActorRotation;

	if (FMath::IsWithin(DeltaRotation.Yaw, AimYawMin, AimYawMax))
	{
		FRotator TargetRot = ActorRotation;
		TargetRot.Yaw += FMath::FInterpTo(ActorRotation.Yaw, ControlRotation.Yaw, GetWorld()->GetDeltaSeconds(),
		                                  InterpSpeed);
		SetActorRotation(TargetRot);
	}
	else
	{
		float ClampedYaw = FMath::Clamp(ControlRotation.Yaw, AimYawMin, AimYawMax);
		FRotator NewRotation = FRotator(ActorRotation.Pitch, ClampedYaw, ActorRotation.Roll);
		SetActorRotation(NewRotation);
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
	float MappedValue = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 300.0f), FVector2D(1.0f, 3.0f), AimYawRate);
	return CurrentMovementSettings.RotationRateCurve->GetFloatValue(GetMappedSpeed()) * MappedValue;
}

bool ABaseCharacter::CanUpdateMovingRotation() const
{
	return ((bIsMoving && bHasMovementInput) || Speed > 150) && !HasAnyRootMotion();
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

	if (DebugTrace == EDrawDebugTrace::Type::ForDuration ||
		DebugTrace == EDrawDebugTrace::Type::ForOneFrame)
	{
		DrawDebugCapsule(
			GetWorld(),
			(StartLocation + EndLocation) * 0.5f,
			CapsuleHalfHeight,
			TraceSettings.ForwardTraceRadius,
			FQuat::Identity,
			bHit ? FColor::Green : FColor::Red,
			false,
			5.0f);
	}

	bool IsWalkable = GetCharacterMovement()->IsWalkable(HitResult);

	if (!IsWalkable && HitResult.bBlockingHit && !HitResult.bStartPenetrating)
	{
		FVector InitialTraceImpactPoint = HitResult.ImpactPoint;
		FVector InitialTraceNormal = HitResult.ImpactNormal;
		return true;
	}

	return false;
}

UAnimMontage* ABaseCharacter::GetGetUpAnimation(bool bRagdollFaceUpState)
{
	return nullptr;
}

void ABaseCharacter::RagdollStart()
{
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetMovementState(EMovementState::Ragdoll);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_Pelvis, true, true);

	if (IsValid(MainAnimInstance))
		MainAnimInstance->Montage_Stop(0.2f);
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

void ABaseCharacter::RagdollUpdate()
{
	GetMesh()->bOnlyAllowAutonomousTickPose = false;

	LastRagdollVelocity = GetMesh()->GetPhysicsLinearVelocity(NAME_root);

	const float SpringValue = FMath::GetMappedRangeValueClamped<float, float>(
		{0.0f, 1000.0f}, {0.0f, 25000.0f}, LastRagdollVelocity.Size());
	GetMesh()->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);

	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	GetMesh()->SetEnableGravity(bEnableGrav);

	SetActorLocationDuringRagdoll();
}


void ABaseCharacter::SetActorLocationDuringRagdoll()
{
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

	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
	                        TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect,
	                                                       ECC_Visibility, Params);

	if (DebugComponent && DebugComponent->GetShowTraces())
	{
		UDebugComponent::DrawDebugLineTraceSingle(GetWorld(),
		                                          TargetRagdollLocation,
		                                          TraceVect,
		                                          EDrawDebugTrace::Type::ForOneFrame,
		                                          bHit,
		                                          HitResult,
		                                          FLinearColor::Red,
		                                          FLinearColor::Green,
		                                          1.0f);
	}

	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;

	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}

	SetActorLocationAndTargetRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation, TargetRagdollRotation,
	                                  false, false);
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
	return GetMesh()->GetSocketLocation(SocketFPCamera);
}

FTransform ABaseCharacter::GetTPPivotTarget()
{
	return GetActorTransform();
}

FTPTraceParams ABaseCharacter::GetTPTraceParams()
{
	return FTPTraceParams(GetActorLocation(), 15, ECC_Camera);
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
