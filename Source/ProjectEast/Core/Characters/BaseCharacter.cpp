#include "BaseCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


class UEnhancedInputLocalPlayerSubsystem;

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	// SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// SpringArmComponent->SetupAttachment(RootComponent);
	// SpringArmComponent->bUsePawnControlRotation = true;
	//
	// CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// CameraComponent->SetupAttachment(SpringArmComponent);
	// CameraComponent->bUsePawnControlRotation = false;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnBeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetEssentialValues(DeltaTime);
	CacheValues();
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
			if (HasMovementInput)
				MantleCheck(FallingTraceSettings, EDrawDebugTrace::Type::ForDuration);
		}
		break;
	case EMovementState::Ragdoll:
		//RagdollUpdate();
		break;
	default: ;
	}
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
		                                   this, &ABaseCharacter::PlayerCrouchInput);
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
				if (HasMovementInput && MantleCheck(GroundedTraceSettings, EDrawDebugTrace::Type::ForDuration))
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

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	EMovementMode NewMovementMode = GetCharacterMovement()->MovementMode;
	OnCharacterMovementModeChanged(NewMovementMode);
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
	GetMesh()->AddTickPrerequisiteActor(this);
	if (IsValid(GetMesh()->GetAnimInstance()))
		MainAnimInstance = GetMesh()->GetAnimInstance();
	SetMovementModel();
	OnGaitChanged(DesiredGait);
	OnRotationModeChanged(DesiredRotationMode);
	OnViewModeChanged(DesiredEViewMode);
	OnOverlayStateChanged(DesiredOverlayState);
	OnStanceChanged(DesiredStance);

	TargetRotation = GetActorRotation();
	LastVelocityRotation = GetActorRotation();
	LastMovementInputRotation = GetActorRotation();
}

void ABaseCharacter::SetMovementModel()
{
	MovementData = MovementModel.DataTable->FindRow<FMovementSettingsState>(MovementModel.RowName,TEXT(""));
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

void ABaseCharacter::OnCharacterMovementModeChanged(EMovementMode NewMovementMode)
{
	if (NewMovementMode == MOVE_Walking || NewMovementMode == MOVE_NavWalking)
		SetMovementState(EMovementState::Grounded);
	if (NewMovementMode == MOVE_Falling)
		SetMovementState(EMovementState::InAir);
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
	if (HasMovementInput)
	{
		const float MinMovementInputThreshold = 0.9f;
		switch (RotationMode)
		{
		case ERotationMode::VelocityDirection:
			return MovementInputAmount > MinMovementInputThreshold;
		case ERotationMode::LookingDirection:
			{
				const float MaxYawDifferenceThreshold = 50.0f;

				FRotator AccelerationRotation = GetCharacterMovement()->GetCurrentAcceleration().Rotation();
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
void ABaseCharacter::SetMovementState(EMovementState NewMovementState)
{
	if (NewMovementState != MovementState)
		OnMovementStateChanged(NewMovementState);
}

void ABaseCharacter::SetMovementAction(EMovementAction NewMovementAction)
{
	if (NewMovementAction != MovementAction)
		OnMovementActionChanged(NewMovementAction);
}

void ABaseCharacter::SetRotationMode(ERotationMode NewRotationMode)
{
	if (NewRotationMode != RotationMode)
		OnRotationModeChanged(NewRotationMode);
}

void ABaseCharacter::SetGait(EGait NewGait)
{
	if (NewGait != Gait)
		OnGaitChanged(NewGait);
}

void ABaseCharacter::SetViewMode(EViewMode NewViewMode)
{
	if (NewViewMode != ViewMode)
		OnViewModeChanged(NewViewMode);
}

void ABaseCharacter::SetOverlayState(EOverlayState NewOverlayState)
{
	if (NewOverlayState != OverlayState)
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
			if (HasMovementInput)
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

TTuple<FHitResult*, bool> ABaseCharacter::CustomSetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation,
	bool bSweep, bool bTeleport)
{
	FHitResult* Result = nullptr;
	TargetRotation = NewRotation;
	auto isSetActor = SetActorLocationAndRotation(NewLocation, TargetRotation, bSweep, Result, bTeleport ? ETeleportType::ResetPhysics : ETeleportType::None);
	return MakeTuple(Result, isSetActor);
}

float ABaseCharacter::CalculateGroundedRotationRate() const
{
	float MappedValue = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 300.0f), FVector2D(1.0f, 3.0f), AimYawRate);
	return CurrentMovementSettings.RotationRateCurve->GetFloatValue(GetMappedSpeed()) * MappedValue;
}

bool ABaseCharacter::CanUpdateMovingRotation() const
{
	return ((IsMoving && HasMovementInput) || Speed > 150) && !HasAnyRootMotion();
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
	EssentialValues.IsMoving = IsMoving;
	EssentialValues.HasMovementInput = HasMovementInput;
	EssentialValues.Speed = Speed;
	EssentialValues.MovementInputAmount = MovementInputAmount;
	EssentialValues.AimingRotation = GetControlRotation();
	EssentialValues.AimYawRate = AimYawRate;
	return EssentialValues;
}

void ABaseCharacter::SetEssentialValues(float DeltaTime)
{
	const FVector LocalCurrentVelocity = GetVelocity();

	Acceleration = (LocalCurrentVelocity - PreviousVelocity) / DeltaTime;
	Speed = LocalCurrentVelocity.Size2D();
	IsMoving = Speed > 1.0f;
	if (IsMoving)
		LastVelocityRotation = LocalCurrentVelocity.ToOrientationRotator();

	FVector LocalCurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
	MovementInputAmount = LocalCurrentAcceleration.Length() / GetCharacterMovement()->GetMaxAcceleration();
	HasMovementInput = MovementInputAmount > 0.0f;

	if (HasMovementInput)
		LastMovementInputRotation = LocalCurrentAcceleration.ToOrientationRotator();

	AimYawRate = FMath::Abs((Controller->GetControlRotation().Yaw - PreviousAimYaw) / DeltaTime);
}

void ABaseCharacter::CacheValues()
{
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = Controller->GetControlRotation().Yaw;
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
		return MainAnimInstance->GetCurveValue(CurveName);
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
