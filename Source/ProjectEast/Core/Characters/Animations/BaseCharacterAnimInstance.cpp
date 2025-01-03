#include "BaseCharacterAnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Characters/BaseCharacter.h"
#include "ProjectEast/Core/Characters/Interfaces/CameraParameters.h"
#include "ProjectEast/Core/Characters/Interfaces/CharacterInfo.h"
#include "ProjectEast/Core/Library/MathLibrary.h"

static const FName NAME_BasePose_CLF(TEXT("BasePose_CLF"));
static const FName NAME_BasePose_N(TEXT("BasePose_N"));
static const FName NAME_Enable_FootIK_R(TEXT("Enable_FootIK_R"));
static const FName NAME_Enable_FootIK_L(TEXT("Enable_FootIK_L"));
static const FName NAME_Enable_HandIK_L(TEXT("Enable_HandIK_L"));
static const FName NAME_Enable_HandIK_R(TEXT("Enable_HandIK_R"));
static const FName NAME_Enable_Transition(TEXT("Enable_Transition"));
static const FName NAME_FootLock_L(TEXT("FootLock_L"));
static const FName NAME_FootLock_R(TEXT("FootLock_R"));
static const FName NAME_Grounded___Slot(TEXT("Grounded Slot"));
static const FName NAME_Layering_Arm_L(TEXT("Layering_Arm_L"));
static const FName NAME_Layering_Arm_L_Add(TEXT("Layering_Arm_L_Add"));
static const FName NAME_Layering_Arm_L_LS(TEXT("Layering_Arm_L_LS"));
static const FName NAME_Layering_Arm_R(TEXT("Layering_Arm_R"));
static const FName NAME_Layering_Arm_R_Add(TEXT("Layering_Arm_R_Add"));
static const FName NAME_Layering_Arm_R_LS(TEXT("Layering_Arm_R_LS"));
static const FName NAME_Layering_Hand_L(TEXT("Layering_Hand_L"));
static const FName NAME_Layering_Hand_R(TEXT("Layering_Hand_R"));
static const FName NAME_Layering_Head_Add(TEXT("Layering_Head_Add"));
static const FName NAME_Layering_Spine_Add(TEXT("Layering_Spine_Add"));
static const FName NAME_Mask_AimOffset(TEXT("Mask_AimOffset"));
static const FName NAME_Mask_LandPrediction(TEXT("Mask_LandPrediction"));
static const FName NAME__ALSCharacterAnimInstance__RotationAmount(TEXT("RotationAmount"));
static const FName NAME_VB___foot_target_l(TEXT("VB foot_target_l"));
static const FName NAME_VB___foot_target_r(TEXT("VB foot_target_r"));
static const FName NAME_Weight_Gait(TEXT("Weight_Gait"));
static const FName NAME__ALSCharacterAnimInstance__root(TEXT("root"));


void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	auto Owner = TryGetPawnOwner();
	if (IsValid(Owner))
	{
		Character = Cast<ABaseCharacter>(Owner);

		if (Character->GetClass()->ImplementsInterface(UCameraParameters::StaticClass()))
			CameraParameters = Cast<ICameraParameters>(Character);
		if (Character->GetClass()->ImplementsInterface(UCharacterInfo::StaticClass()))
			CharacterInfo = Cast<ICharacterInfo>(Character);
	}
}


void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	DeltaTimeX = DeltaSeconds;
	if (!Character || DeltaSeconds == 0.0f)
	{
		return;
	}
	

	// Update rest of character information. Others are reflected into anim bp when they're set inside character class
	CharacterInformation.MovementInputAmount = Character->GetMovementInputAmount();
	CharacterInformation.bHasMovementInput = Character->HasMovementInput();
	CharacterInformation.bIsMoving = Character->IsMoving();
	CharacterInformation.Acceleration = Character->GetAcceleration();
	CharacterInformation.AimYawRate = Character->GetAimYawRate();
	CharacterInformation.Speed = Character->GetSpeed();
	CharacterInformation.Velocity = Character->GetCharacterMovement()->Velocity;
	CharacterInformation.MovementInput = Character->GetMovementInput();
	CharacterInformation.AimingRotation = Character->GetAimingRotation();
	CharacterInformation.CharacterActorRotation = Character->GetActorRotation();
	CharacterInformation.ViewMode = Character->GetViewMode();
	CharacterInformation.ViewMode = Character->GetViewMode();
	CharacterInformation.PrevMovementState = Character->GetPrevMovementState();
	LayerBlendingValues.OverlayOverrideState = Character->GetOverlayOverrideState();
	MovementState = Character->GetMovementState();
	
	MovementAction = Character->GetMovementAction();
	Stance = Character->GetStance();
	RotationMode = Character->GetRotationMode();
	Gait = Character->GetGait();
	OverlayState = Character->GetOverlayState();
	GroundedEntryState = Character->GetGroundedEntryState();

	UpdateAimingValues(DeltaSeconds);
	UpdateLayerValues();
	UpdateWallRunParameters(DeltaSeconds);
	UpdateFootIK(DeltaSeconds);

	if (MovementState == EMovementState::Grounded)
	{
		// Check If Moving Or Not & Enable Movement Animations if IsMoving and HasMovementInput, or if the Speed is greater than 150.
		const bool bPrevShouldMove = Grounded.bShouldMove;
		Grounded.bShouldMove = ShouldMoveCheck();

		if (bPrevShouldMove == false && Grounded.bShouldMove)
		{
			// Do When Starting To Move
			TurnInPlaceValues.ElapsedDelayTime = 0.0f;
			Grounded.bRotateL = false;
			Grounded.bRotateR = false;
		}

		if (Grounded.bShouldMove)
		{
			// Do While Moving
			UpdateMovementValues(DeltaSeconds);
			UpdateRotationValues();
		}
		else
		{
			// Do While Not Moving
			if (CanRotateInPlace())
			{
				RotateInPlaceCheck();
			}
			else
			{
				Grounded.bRotateL = false;
				Grounded.bRotateR = false;
			}
			if (CanTurnInPlace())
			{
				TurnInPlaceCheck();
			}
			else
			{
				TurnInPlaceValues.ElapsedDelayTime = 0.0f;
			}
			if (CanDynamicTransition())
			{
				DynamicTransitionCheck();
			}
		}
	}
	else if (MovementState == EMovementState::InAir)
	{
		UpdateInAirValues();
	}
	else if (MovementState == EMovementState::Ragdoll)
	{
		UpdateRagdollValues();
	}
}

void UBaseCharacterAnimInstance::PlayTransition(const FALSDynamicMontageParams& Parameters)
{
	PlaySlotAnimationAsDynamicMontage(Parameters.Animation, NAME_Grounded___Slot,
	                                  Parameters.BlendInTime, Parameters.BlendOutTime, Parameters.PlayRate, 1,
	                                  0.0f, Parameters.StartTime);
}

void UBaseCharacterAnimInstance::PlayTransitionChecked(const FALSDynamicMontageParams& Parameters)
{
	if (Stance == EStance::Standing && !Grounded.bShouldMove)
	{
		PlayTransition(Parameters);
	}
}

void UBaseCharacterAnimInstance::PlayDynamicTransition(float ReTriggerDelay, FALSDynamicMontageParams Parameters)
{
	if (bCanPlayDynamicTransition)
	{
		bCanPlayDynamicTransition = false;

		PlayTransition(Parameters);

		UWorld* World = GetWorld();
		check(World);
		World->GetTimerManager().SetTimer(PlayDynamicTransitionTimer, this,
		                                  &UBaseCharacterAnimInstance::PlayDynamicTransitionDelay,
		                                  ReTriggerDelay, false);
	}
}

void UBaseCharacterAnimInstance::OnJumped()
{
	InAir.bJumped = true;
	InAir.JumpPlayRate = FMath::GetMappedRangeValueClamped<float, float>(
		{0.0f, 600.0f}, {1.2f, 1.5f}, CharacterInformation.Speed);

	GetWorld()->GetTimerManager().SetTimer(OnJumpedTimer, this,
	                                       &UBaseCharacterAnimInstance::OnJumpedDelay, 0.1f, false);
}

void UBaseCharacterAnimInstance::OnPivot()
{
	Grounded.bPivot = CharacterInformation.Speed < Config.TriggerPivotSpeedLimit;
	GetWorld()->GetTimerManager().SetTimer(OnPivotTimer, this,
	                                       &UBaseCharacterAnimInstance::OnPivotDelay, 0.1f, false);
}

// void UBaseCharacterAnimInstance::UpdateCharacterInfo()
// {
// 	auto EssentialValues = CharacterInfo->GetEssentialValues();
// 	auto CurrentStates = CharacterInfo->GetCurrentStates();
//
// 	CharacterInformation.Velocity = EssentialValues.Velocity;
// 	CharacterInformation.Acceleration = EssentialValues.Acceleration;
// 	CharacterInformation.MovementInput = EssentialValues.MovementInput;
// 	CharacterInformation.bIsMoving = EssentialValues.IsMoving;
// 	CharacterInformation.bHasMovementInput = EssentialValues.HasMovementInput;
// 	CharacterInformation.Speed = EssentialValues.Speed;
// 	CharacterInformation.MovementInputAmount = EssentialValues.MovementInputAmount;
// 	CharacterInformation.AimYawRate = EssentialValues.AimYawRate;
// 	CharacterInformation.AimingRotation = EssentialValues.AimingRotation;
//
// 	MovementState = CurrentStates.MovementState;
// 	PrevMovementState = CurrentStates.PrevMovementState;
// 	MovementAction = CurrentStates.MovementAction;
// 	RotationMode = CurrentStates.RotationMode;
// 	Gait = CurrentStates.ActualGait;
// 	Stance = CurrentStates.ActualStance;
// 	ViewMode = CurrentStates.ViewMode;
// 	OverlayState = CurrentStates.OverlayState;
// }

void UBaseCharacterAnimInstance::UpdateAimingValues(float DeltaSeconds)
{
	AimingValues.SmoothedAimingRotation = FMath::RInterpTo(AimingValues.SmoothedAimingRotation,
	                                                       CharacterInformation.AimingRotation, DeltaSeconds,
	                                                       Config.SmoothedAimingRotationInterpSpeed);

	FRotator Delta = CharacterInformation.AimingRotation - CharacterInformation.CharacterActorRotation;
	Delta.Normalize();
	AimingValues.AimingAngle.X = Delta.Yaw;
	AimingValues.AimingAngle.Y = Delta.Pitch;

	Delta = AimingValues.SmoothedAimingRotation - CharacterInformation.CharacterActorRotation;
	Delta.Normalize();
	SmoothedAimingAngle.X = Delta.Yaw;
	SmoothedAimingAngle.Y = Delta.Pitch;

	if (RotationMode != ERotationMode::VelocityDirection)
	{
		AimingValues.AimSweepTime = FMath::GetMappedRangeValueClamped<float, float>({-90.0f, 90.0f}, {1.0f, 0.0f},
			AimingValues.AimingAngle.Y);

		AimingValues.SpineRotation.Roll = 0.0f;
		AimingValues.SpineRotation.Pitch = 0.0f;
		AimingValues.SpineRotation.Yaw = AimingValues.AimingAngle.X / 4.0f;
	}
	else
	{
		if (CharacterInformation.bHasMovementInput)
		{
			Delta = CharacterInformation.MovementInput.ToOrientationRotator() - CharacterInformation.
				CharacterActorRotation;
			Delta.Normalize();
			const float InterpTarget = FMath::GetMappedRangeValueClamped<float, float>(
				{-180.0f, 180.0f}, {0.0f, 1.0f}, Delta.Yaw);

			AimingValues.InputYawOffsetTime = FMath::FInterpTo(AimingValues.InputYawOffsetTime, InterpTarget,
			                                                   DeltaSeconds, Config.InputYawOffsetInterpSpeed);
		}
	}

	AimingValues.LeftYawTime = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 180.0f}, {0.5f, 0.0f},
	                                                                           FMath::Abs(SmoothedAimingAngle.X));
	AimingValues.RightYawTime = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 180.0f}, {0.5f, 1.0f},
		FMath::Abs(SmoothedAimingAngle.X));
	AimingValues.ForwardYawTime = FMath::GetMappedRangeValueClamped<float, float>({-180.0f, 180.0f}, {0.0f, 1.0f},
		SmoothedAimingAngle.X);
}

void UBaseCharacterAnimInstance::UpdateLayerValues()
{
	LayerBlendingValues.EnableAimOffset = FMath::Lerp(1.0f, 0.0f, GetCurveValue(NAME_Mask_AimOffset));

	LayerBlendingValues.BasePose_N = GetCurveValue(NAME_BasePose_N);
	LayerBlendingValues.BasePose_CLF = GetCurveValue(NAME_BasePose_CLF);

	LayerBlendingValues.Spine_Add = GetCurveValue(NAME_Layering_Spine_Add);
	LayerBlendingValues.Head_Add = GetCurveValue(NAME_Layering_Head_Add);
	LayerBlendingValues.Arm_L_Add = GetCurveValue(NAME_Layering_Arm_L_Add);
	LayerBlendingValues.Arm_R_Add = GetCurveValue(NAME_Layering_Arm_R_Add);

	LayerBlendingValues.Hand_R = GetCurveValue(NAME_Layering_Hand_R);
	LayerBlendingValues.Hand_L = GetCurveValue(NAME_Layering_Hand_L);
	LayerBlendingValues.EnableHandIK_L = FMath::Lerp(0.0f, GetCurveValue(NAME_Enable_HandIK_L),
	                                                 GetCurveValue(NAME_Layering_Arm_L));
	LayerBlendingValues.EnableHandIK_R = FMath::Lerp(0.0f, GetCurveValue(NAME_Enable_HandIK_R),
	                                                 GetCurveValue(NAME_Layering_Arm_R));

	LayerBlendingValues.Arm_L_LS = GetCurveValue(NAME_Layering_Arm_L_LS);
	LayerBlendingValues.Arm_L_MS = static_cast<float>(1 - FMath::FloorToInt(LayerBlendingValues.Arm_L_LS));
	LayerBlendingValues.Arm_R_LS = GetCurveValue(NAME_Layering_Arm_R_LS);
	LayerBlendingValues.Arm_R_MS = static_cast<float>(1 - FMath::FloorToInt(LayerBlendingValues.Arm_R_LS));
}


void UBaseCharacterAnimInstance::UpdateWallRunParameters(float DeltaSeconds)
{
	bIsWallRunning = Character->GetWallRunComponent()->GetIsWallRunning();
	bIsWallJumping = Character->GetWallRunComponent()->GetIsWallJumping();
	DirectionType = Character->GetWallRunComponent()->GetDirectionType();
	CurrentArcAngle = Character->GetWallRunComponent()->GetCurrentArcAngle();
	CurrentTurnRate = Character->GetWallRunComponent()->GetCurrentTurnRate();
	CurrentAnimPlayRate = UKismetMathLibrary::MapRangeClamped(Character->GetMovementComponent()->Velocity.Length(), 680.0f, 920.0f, 0.85f, 1.15f);

	float SizeSpeed = FVector(0.0f, Character->GetWallRunComponent()->GetWallJumpHorizontalVelocity(),Character->GetWallRunComponent()->GetWallJumpVerticalVelocity()).Length();
	WallJumpAnimationSpeed = UKismetMathLibrary::MapRangeClamped(SizeSpeed, 943.0f, 1500.0f, 1.0f,1.5f);


	float TargetHandCorrectionAdditive = CurrentArcAngle >= 15? UKismetMathLibrary::MapRangeClamped(CurrentArcAngle, 30.0f, 45.0f, 1.0f, 0.0f)
	: UKismetMathLibrary::MapRangeClamped(CurrentArcAngle, 0.0f, 30.0f, 0.0f, 1.0f);

	HandCorrectionAdditive = UKismetMathLibrary::FInterpTo(HandCorrectionAdditive, TargetHandCorrectionAdditive,DeltaSeconds, 7.0f);
	
}

void UBaseCharacterAnimInstance::UpdateFootIK(float DeltaSeconds)
{
	FVector FootOffsetRTarget = FVector::ZeroVector;
	FVector FootOffsetLTarget = FVector::ZeroVector;

	SetFootLocking(DeltaSeconds, NAME_Enable_FootIK_L, NAME_FootLock_L,
	               IkFootL_BoneName, FootIKValues.FootLock_L_Alpha, FootIKValues.UseFootLockCurve_L,
	               FootIKValues.FootLock_L_Location, FootIKValues.FootLock_L_Rotation);
	SetFootLocking(DeltaSeconds, NAME_Enable_FootIK_R, NAME_FootLock_R,
	               IkFootR_BoneName, FootIKValues.FootLock_R_Alpha, FootIKValues.UseFootLockCurve_R,
	               FootIKValues.FootLock_R_Location, FootIKValues.FootLock_R_Rotation);

	if (MovementState == EMovementState::InAir)
	{
		SetPelvisIKOffset(DeltaSeconds, FVector::ZeroVector, FVector::ZeroVector);
		ResetIKOffsets(DeltaSeconds);
	}
	else if (MovementState != EMovementState::Ragdoll)
	{
		SetFootOffsets(DeltaSeconds, NAME_Enable_FootIK_L, IkFootL_BoneName, NAME__ALSCharacterAnimInstance__root,
		               FootOffsetLTarget, FootIKValues.FootOffset_L_Location, FootIKValues.FootOffset_L_Rotation);
		SetFootOffsets(DeltaSeconds, NAME_Enable_FootIK_R, IkFootR_BoneName, NAME__ALSCharacterAnimInstance__root,
		               FootOffsetRTarget, FootIKValues.FootOffset_R_Location, FootIKValues.FootOffset_R_Rotation);
		SetPelvisIKOffset(DeltaSeconds, FootOffsetLTarget, FootOffsetRTarget);
	}
}

void UBaseCharacterAnimInstance::UpdateMovementValues(float DeltaSeconds)
{
	InterpVelocityBlend(CalculateVelocityBlend(), DeltaSeconds);

	Grounded.DiagonalScaleAmount = CalculateDiagonalScaleAmount();
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();

	FALSLeanAmount TargetLeanAmount = FALSLeanAmount(RelativeAccelerationAmount.Y, RelativeAccelerationAmount.X);
	LeanAmount = InterpLeanAmount(LeanAmount, TargetLeanAmount, Config.GroundedLeanInterpSpeed, DeltaSeconds);

	Grounded.WalkRunBlend = CalculateWalkRunBlend();
	Grounded.StrideBlend = CalculateStrideBlend();
	Grounded.StandingPlayRate = CalculateStandingPlayRate();
	Grounded.CrouchingPlayRate = CalculateCrouchingPlayRate();
}

void UBaseCharacterAnimInstance::UpdateRotationValues()
{
	MovementDirection = CalculateMovementDirection();

	FRotator Delta = CharacterInformation.Velocity.ToOrientationRotator() - CharacterInformation.AimingRotation;
	Delta.Normalize();

	const FVector& FBOffset = YawOffset_FB->GetVectorValue(Delta.Yaw);
	Grounded.FYaw = FBOffset.X;
	Grounded.BYaw = FBOffset.Y;

	const FVector& LROffset = YawOffset_LR->GetVectorValue(Delta.Yaw);
	Grounded.LYaw = LROffset.X;
	Grounded.RYaw = LROffset.Y;
}

void UBaseCharacterAnimInstance::UpdateInAirValues()
{
	InAir.FallSpeed = CharacterInformation.Velocity.Z;
	InAir.LandPrediction = CalculateLandPrediction();
	LeanAmount = InterpLeanAmount(LeanAmount, CalculateAirLeanAmount(), Config.InAirLeanInterpSpeed, DeltaTimeX);
}

void UBaseCharacterAnimInstance::UpdateRagdollValues()
{
	float Length = GetOwningComponent()->GetPhysicsLinearVelocity(NAME__ALSCharacterAnimInstance__root).Length();
	FlailRate = FMath::GetMappedRangeValueClamped(FVector2D(0, 1000), FVector2D(0, 1), Length);
}

bool UBaseCharacterAnimInstance::ShouldMoveCheck() const
{
	return (CharacterInformation.bIsMoving && CharacterInformation.bHasMovementInput) || CharacterInformation.Speed >
		150;
}

bool UBaseCharacterAnimInstance::CanTurnInPlace() const
{
	return RotationMode == ERotationMode::LookingDirection && ViewMode == EViewMode::ThirdPerson &&
		GetCurveValue("Enable_Transition") > 0.99f;
}

bool UBaseCharacterAnimInstance::CanRotateInPlace() const
{
	return RotationMode == ERotationMode::Aiming || ViewMode == EViewMode::FirstPerson;
}

bool UBaseCharacterAnimInstance::CanDynamicTransition() const
{
	return GetCurveValue("Enable_Transition") == 1.0f;
}

bool UBaseCharacterAnimInstance::CanOverlayTransition() const
{
	return Stance == EStance::Standing && !Grounded.bShouldMove;
}

void UBaseCharacterAnimInstance::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime,
                                             bool OverrideCurrent)
{
	FRotator Delta = TargetRotation - CharacterInformation.CharacterActorRotation;
	Delta.Normalize();
	const float TurnAngle = Delta.Yaw;

	FALSTurnInPlaceAsset TargetTurnAsset;
	bool IsLeftSide = TurnAngle < 0;

	if (FMath::Abs(TurnAngle) < TurnInPlaceValues.Turn180Threshold)
	{
		if (Stance == EStance::Standing)
			TargetTurnAsset = IsLeftSide ? TurnInPlaceValues.N_TurnIP_L90 : TurnInPlaceValues.N_TurnIP_R90;
		if (Stance == EStance::Crouching)
			TargetTurnAsset = IsLeftSide ? TurnInPlaceValues.CLF_TurnIP_L90 : TurnInPlaceValues.CLF_TurnIP_R90;
	}
	else
	{
		if (Stance == EStance::Standing)
			TargetTurnAsset = IsLeftSide ? TurnInPlaceValues.N_TurnIP_L180 : TurnInPlaceValues.N_TurnIP_R180;
		if (Stance == EStance::Crouching)
			TargetTurnAsset = IsLeftSide ? TurnInPlaceValues.CLF_TurnIP_L180 : TurnInPlaceValues.CLF_TurnIP_R180;
	}


	if (!OverrideCurrent && IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName))
		return;

	PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, 0.2f, 0.2f,
	                                  TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.0f, StartTime);
	if (TargetTurnAsset.ScaleTurnAngle)
		Grounded.RotationScale = (TurnAngle / TargetTurnAsset.AnimatedAngle) * TargetTurnAsset.PlayRate * PlayRateScale;
	else
		Grounded.RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
}

void UBaseCharacterAnimInstance::TurnInPlaceCheck()
{
	if (FMath::Abs(AimingValues.AimingAngle.X) <= TurnInPlaceValues.TurnCheckMinAngle ||
		CharacterInformation.AimYawRate >= TurnInPlaceValues.AimYawRateLimit)
	{
		TurnInPlaceValues.ElapsedDelayTime = 0.0f;
		return;
	}

	TurnInPlaceValues.ElapsedDelayTime += DeltaTimeX;
	const float ClampedAimAngle = FMath::GetMappedRangeValueClamped<float, float>(
		{TurnInPlaceValues.TurnCheckMinAngle, 180.0f},
		{TurnInPlaceValues.MinAngleDelay, TurnInPlaceValues.MaxAngleDelay},
		AimingValues.AimingAngle.X);

	if (TurnInPlaceValues.ElapsedDelayTime > ClampedAimAngle)
	{
		FRotator TurnInPlaceYawRot = CharacterInformation.AimingRotation;
		TurnInPlaceYawRot.Roll = 0.0f;
		TurnInPlaceYawRot.Pitch = 0.0f;
		TurnInPlace(TurnInPlaceYawRot, 1.0f, 0.0f, false);
	}
}

void UBaseCharacterAnimInstance::RotateInPlaceCheck()
{
	Grounded.bRotateL = AimingValues.AimingAngle.X < RotateInPlace.RotateMinThreshold;
	Grounded.bRotateR = AimingValues.AimingAngle.X > RotateInPlace.RotateMaxThreshold;

	if (Grounded.bRotateL || Grounded.bRotateR)
	{
		Grounded.RotateRate = FMath::GetMappedRangeValueClamped<float, float>(
			{RotateInPlace.AimYawRateMinRange, RotateInPlace.AimYawRateMaxRange},
			{RotateInPlace.MinPlayRate, RotateInPlace.MaxPlayRate},
			CharacterInformation.AimYawRate);
	}
}

void UBaseCharacterAnimInstance::DynamicTransitionCheck()
{
	FTransform SocketTransformA = GetOwningComponent()->GetSocketTransform(IkFootL_BoneName, RTS_Component);
	FTransform SocketTransformB = GetOwningComponent()->GetSocketTransform(NAME_VB___foot_target_l, RTS_Component);
	float Distance = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();

	if (Distance > Config.DynamicTransitionThreshold)
	{
		FALSDynamicMontageParams Params;
		Params.Animation = TransitionAnim_R;
		Params.BlendInTime = 0.2f;
		Params.BlendOutTime = 0.2f;
		Params.PlayRate = 1.5f;
		Params.StartTime = 0.8f;
		PlayDynamicTransition(0.1f, Params);
	}

	SocketTransformA = GetOwningComponent()->GetSocketTransform(IkFootR_BoneName, RTS_Component);
	SocketTransformB = GetOwningComponent()->GetSocketTransform(NAME_VB___foot_target_r, RTS_Component);
	Distance = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();
	if (Distance > Config.DynamicTransitionThreshold)
	{
		FALSDynamicMontageParams Params;
		Params.Animation = TransitionAnim_L;
		Params.BlendInTime = 0.2f;
		Params.BlendOutTime = 0.2f;
		Params.PlayRate = 1.5f;
		Params.StartTime = 0.8f;
		PlayDynamicTransition(0.1f, Params);
	}
}

FALSVelocityBlend UBaseCharacterAnimInstance::CalculateVelocityBlend() const
{
	FVector LocRelativeVelocityDir = CharacterInformation.CharacterActorRotation.UnrotateVector(
		CharacterInformation.Velocity.GetSafeNormal(0.1f));
	float Sum =FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) + FMath::Abs(LocRelativeVelocityDir.Z);

	const FVector RelativeDirection = LocRelativeVelocityDir / Sum;

	FALSVelocityBlend Result;
	Result.F = FMath::Clamp(RelativeDirection.X, 0.0f, 1.0f);
	Result.B = FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f));
	Result.L = FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f));
	Result.R = FMath::Clamp(RelativeDirection.Y, 0.0f, 1.0f);
	return Result;
}

float UBaseCharacterAnimInstance::CalculateDiagonalScaleAmount() const
{
	return DiagonalScaleAmountCurve->GetFloatValue(FMath::Abs(VelocityBlend.B + VelocityBlend.F));
}

FVector UBaseCharacterAnimInstance::CalculateRelativeAccelerationAmount() const
{
	if (FVector::DotProduct(CharacterInformation.Acceleration, CharacterInformation.Velocity) > 0.0f)
	{
		const float MaxAcc = Character->GetCharacterMovement()->GetMaxAcceleration();
		return CharacterInformation.CharacterActorRotation.UnrotateVector(
			CharacterInformation.Acceleration.GetClampedToMaxSize(MaxAcc) / MaxAcc);
	}
	const float MaxBrakingDec = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
	return CharacterInformation.CharacterActorRotation.UnrotateVector(
		CharacterInformation.Acceleration.GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}

float UBaseCharacterAnimInstance::CalculateWalkRunBlend() const
{
	return Gait == EGait::Walking ? 0.0f : 1.0;
}

float UBaseCharacterAnimInstance::CalculateStrideBlend() const
{
	const float CurveTime = CharacterInformation.Speed / GetOwningComponent()->GetComponentScale().Z;
	const float ClampedGait = GetAnimCurveClamped(FName("Weight_Gait"), -1.0, 0.0f, 1.0f);
	const float LerpedStrideBlend = FMath::Lerp(StrideBlend_N_Walk->GetFloatValue(CurveTime),
	                                            StrideBlend_N_Run->GetFloatValue(CurveTime),
	                                            ClampedGait);
	return FMath::Lerp(LerpedStrideBlend, StrideBlend_C_Walk->GetFloatValue(CharacterInformation.Speed),
	                   GetCurveValue(NAME_BasePose_CLF));
}

float UBaseCharacterAnimInstance::CalculateStandingPlayRate() const
{
	const float LerpedSpeed = FMath::Lerp(CharacterInformation.Speed / Config.AnimatedWalkSpeed,
	                                      CharacterInformation.Speed / Config.AnimatedRunSpeed,
	                                      GetAnimCurveClamped(FName("Weight_Gait"), -1.0f, 0.0f, 1.0f));

	const float SprintAffectedSpeed = FMath::Lerp(LerpedSpeed, CharacterInformation.Speed / Config.AnimatedSprintSpeed,
	                                              GetAnimCurveClamped(FName("Weight_Gait"), -2.0f, 0.0f, 1.0f));

	return FMath::Clamp((SprintAffectedSpeed / Grounded.StrideBlend) / GetOwningComponent()->GetComponentScale().Z,
	                    0.0f, 3.0f);
}

float UBaseCharacterAnimInstance::CalculateCrouchingPlayRate() const
{
	return FMath::Clamp(
		CharacterInformation.Speed / Config.AnimatedCrouchSpeed / Grounded.StrideBlend / GetOwningComponent()->
		GetComponentScale().Z,
		0.0f, 2.0f);
}

float UBaseCharacterAnimInstance::CalculateLandPrediction() const
{
	if (InAir.FallSpeed >= -200.0f)
	{
		return 0.0f;
	}

	const UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
	const FVector& CapsuleWorldLoc = CapsuleComp->GetComponentLocation();
	const float VelocityZ = CharacterInformation.Velocity.Z;
	FVector VelocityClamped = CharacterInformation.Velocity;
	VelocityClamped.Z = FMath::Clamp(VelocityZ, -4000.0f, -200.0f);
	VelocityClamped.Normalize();

	const FVector TraceLength = VelocityClamped * FMath::GetMappedRangeValueClamped<float, float>(
		{0.0f, -4000.0f}, {50.0f, 2000.0f}, VelocityZ);

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	FHitResult HitResult;
	const FCollisionShape CapsuleCollisionShape = FCollisionShape::MakeCapsule(CapsuleComp->GetUnscaledCapsuleRadius(),
	                                                                           CapsuleComp->
	                                                                           GetUnscaledCapsuleHalfHeight());
	const bool bHit = World->SweepSingleByChannel(HitResult, CapsuleWorldLoc, CapsuleWorldLoc + TraceLength,
	                                              FQuat::Identity,
	                                              ECC_Visibility, CapsuleCollisionShape, Params);

	// if (ALSDebugComponent && ALSDebugComponent->GetShowTraces())
	// {
	// 	UALSDebugComponent::DrawDebugCapsuleTraceSingle(World,
	// 	                                                CapsuleWorldLoc,
	// 	                                                CapsuleWorldLoc + TraceLength,
	// 	                                                CapsuleCollisionShape,
	// 	                                                EDrawDebugTrace::Type::ForOneFrame,
	// 	                                                bHit,
	// 	                                                HitResult,
	// 	                                                FLinearColor::Red,
	// 	                                                FLinearColor::Green,
	// 	                                                5.0f);
	// }

	if (Character->GetCharacterMovement()->IsWalkable(HitResult))
	{
		return FMath::Lerp(LandPredictionCurve->GetFloatValue(HitResult.Time), 0.0f,
		                   GetCurveValue(NAME_Mask_LandPrediction));
	}

	return 0.0f;
}

FALSLeanAmount UBaseCharacterAnimInstance::CalculateAirLeanAmount() const
{
	FALSLeanAmount CalcLeanAmount;
	const FVector& UnrotatedVel = CharacterInformation.CharacterActorRotation.UnrotateVector(
		CharacterInformation.Velocity) / 350.0f;
	FVector2D InversedVect(UnrotatedVel.Y, UnrotatedVel.X);
	InversedVect *= LeanInAirCurve->GetFloatValue(InAir.FallSpeed);
	CalcLeanAmount.LR = InversedVect.X;
	CalcLeanAmount.FB = InversedVect.Y;
	return CalcLeanAmount;
}

void UBaseCharacterAnimInstance::SetFootOffsets(float DeltaSeconds, FName EnableFootIKCurve, FName IKFootBone,
                                                FName RootBone, FVector& CurLocationTarget, FVector& CurLocationOffset,
                                                FRotator& CurRotationOffset)
{
	if (GetCurveValue(EnableFootIKCurve) <= 0)
	{
		CurLocationOffset = FVector::ZeroVector;
		CurRotationOffset = FRotator::ZeroRotator;
		return;
	}

	USkeletalMeshComponent* OwnerComp = GetOwningComponent();
	FVector IKFootFloorLoc = OwnerComp->GetSocketLocation(IKFootBone);
	IKFootFloorLoc.Z = OwnerComp->GetSocketLocation(RootBone).Z;

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	const FVector TraceStart = IKFootFloorLoc + FVector(0.0, 0.0, Config.IK_TraceDistanceAboveFoot);
	const FVector TraceEnd = IKFootFloorLoc - FVector(0.0, 0.0, Config.IK_TraceDistanceBelowFoot);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult,
	                                                  TraceStart,
	                                                  TraceEnd,
	                                                  ECC_Visibility, Params);

	// if (UDebugComponent && ALSDebugComponent->GetShowTraces())
	// {
	// 	UDebugComponent::DrawDebugLineTraceSingle(
	// 		World,
	// 		TraceStart,
	// 		TraceEnd,
	// 		EDrawDebugTrace::Type::ForOneFrame,
	// 		bHit,
	// 		HitResult,
	// 		FLinearColor::Red,
	// 		FLinearColor::Green,
	// 		5.0f);
	// }

	FRotator TargetRotOffset = FRotator::ZeroRotator;
	if (Character->GetCharacterMovement()->IsWalkable(HitResult))
	{
		FVector ImpactPoint = HitResult.ImpactPoint;
		FVector ImpactNormal = HitResult.ImpactNormal;

		CurLocationTarget = (ImpactPoint + ImpactNormal * Config.FootHeight) -
			(IKFootFloorLoc + FVector(0, 0, Config.FootHeight));

		TargetRotOffset.Pitch = -FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.X, ImpactNormal.Z));
		TargetRotOffset.Roll = FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.Y, ImpactNormal.Z));
	}

	const float InterpSpeed = CurLocationOffset.Z > CurLocationTarget.Z ? 30.f : 15.0f;
	CurLocationOffset = FMath::VInterpTo(CurLocationOffset, CurLocationTarget, DeltaSeconds, InterpSpeed);

	CurRotationOffset = FMath::RInterpTo(CurRotationOffset, TargetRotOffset, DeltaSeconds, 30.0f);
}

void UBaseCharacterAnimInstance::SetPelvisIKOffset(float DeltaSeconds, FVector FootOffsetLTarget,
                                                   FVector FootOffsetRTarget)
{
	FootIKValues.PelvisAlpha =
		(GetCurveValue(NAME_Enable_FootIK_L) + GetCurveValue(NAME_Enable_FootIK_R)) / 2.0f;

	if (FootIKValues.PelvisAlpha > 0.0f)
	{
		const FVector PelvisTarget = FootOffsetLTarget.Z < FootOffsetRTarget.Z ? FootOffsetLTarget : FootOffsetRTarget;
		const float InterpSpeed = PelvisTarget.Z > FootIKValues.PelvisOffset.Z ? 10.0f : 15.0f;
		FootIKValues.PelvisOffset =
			FMath::VInterpTo(FootIKValues.PelvisOffset, PelvisTarget, DeltaSeconds, InterpSpeed);
	}
	else
	{
		FootIKValues.PelvisOffset = FVector::ZeroVector;
	}
}

void UBaseCharacterAnimInstance::SetFootLocking(float DeltaSeconds, FName EnableFootIKCurve, FName FootLockCurve,
                                                FName IKFootBone, float& CurFootLockAlpha, bool& UseFootLockCurve,
                                                FVector& CurFootLockLoc, FRotator& CurFootLockRot)
{
	if (GetCurveValue(EnableFootIKCurve) <= 0.0f)
		return;

	float FootLockCurveVal;

	if (UseFootLockCurve)
	{
		UseFootLockCurve = FMath::Abs(GetCurveValue(NAME__ALSCharacterAnimInstance__RotationAmount)) <= 0.001f ||
			Character->GetLocalRole() != ROLE_AutonomousProxy;
		FootLockCurveVal = GetCurveValue(FootLockCurve) * (1.f / GetSkelMeshComponent()->AnimUpdateRateParams->
			UpdateRate);
	}
	else
	{
		UseFootLockCurve = GetCurveValue(FootLockCurve) >= 0.99f;
		FootLockCurveVal = 0.0f;
	}

	if (FootLockCurveVal >= 0.99f || FootLockCurveVal < CurFootLockAlpha)
	{
		CurFootLockAlpha = FootLockCurveVal;
	}

	if (CurFootLockAlpha >= 0.99f)
	{
		const FTransform& OwnerTransform =
			GetOwningComponent()->GetSocketTransform(IKFootBone, RTS_Component);
		CurFootLockLoc = OwnerTransform.GetLocation();
		CurFootLockRot = OwnerTransform.Rotator();
	}

	if (CurFootLockAlpha > 0.0f)
	{
		SetFootLockOffsets(DeltaSeconds, CurFootLockLoc, CurFootLockRot);
	}
}

void UBaseCharacterAnimInstance::SetFootLockOffsets(float DeltaSeconds, FVector& LocalLocation,
                                                    FRotator& LocalRotation) const
{
	FRotator RotationDifference = FRotator::ZeroRotator;
	if (Character->GetCharacterMovement()->IsMovingOnGround())
	{
		RotationDifference = CharacterInformation.CharacterActorRotation - Character->GetCharacterMovement()->
			GetLastUpdateRotation();
		RotationDifference.Normalize();
	}


	const FVector& LocationDifference = GetOwningComponent()->GetComponentRotation().UnrotateVector(
		CharacterInformation.Velocity * DeltaSeconds);

	LocalLocation = (LocalLocation - LocationDifference).RotateAngleAxis(RotationDifference.Yaw, FVector::DownVector);
	FRotator Delta = LocalRotation - RotationDifference;
	Delta.Normalize();
	LocalRotation = Delta;
}

void UBaseCharacterAnimInstance::ResetIKOffsets(float DeltaSeconds)
{
	FootIKValues.FootOffset_L_Location = FMath::VInterpTo(FootIKValues.FootOffset_L_Location,
	                                                      FVector::ZeroVector, DeltaSeconds, 15.0f);
	FootIKValues.FootOffset_R_Location = FMath::VInterpTo(FootIKValues.FootOffset_R_Location,
	                                                      FVector::ZeroVector, DeltaSeconds, 15.0f);
	FootIKValues.FootOffset_L_Rotation = FMath::RInterpTo(FootIKValues.FootOffset_L_Rotation,
	                                                      FRotator::ZeroRotator, DeltaSeconds, 15.0f);
	FootIKValues.FootOffset_R_Rotation = FMath::RInterpTo(FootIKValues.FootOffset_R_Rotation,
	                                                      FRotator::ZeroRotator, DeltaSeconds, 15.0f);
}

EMovementDirection UBaseCharacterAnimInstance::CalculateMovementDirection() const
{
	if (Gait == EGait::Sprinting || RotationMode == ERotationMode::VelocityDirection)
		return EMovementDirection::Forward;

	FRotator Delta = CharacterInformation.Velocity.ToOrientationRotator() - CharacterInformation.AimingRotation;
	Delta.Normalize();
	return UMathLibrary::CalculateQuadrant(MovementDirection, 70.0f, -70.0f, 110.0f, -110.0f, 5.0f, Delta.Yaw);
}


FALSVelocityBlend UBaseCharacterAnimInstance::InterpVelocityBlend(FALSVelocityBlend Target, float DeltaTime)
{
	VelocityBlend.F = FMath::FInterpTo(VelocityBlend.F, Target.F, DeltaTime, Config.VelocityBlendInterpSpeed);
	VelocityBlend.B = FMath::FInterpTo(VelocityBlend.B, Target.B, DeltaTime, Config.VelocityBlendInterpSpeed);
	VelocityBlend.L = FMath::FInterpTo(VelocityBlend.L, Target.L, DeltaTime, Config.VelocityBlendInterpSpeed);
	VelocityBlend.R = FMath::FInterpTo(VelocityBlend.R, Target.R, DeltaTime, Config.VelocityBlendInterpSpeed);

	return VelocityBlend;
}

FALSLeanAmount UBaseCharacterAnimInstance::InterpLeanAmount(FALSLeanAmount Current, FALSLeanAmount Target,
                                                            float InterpSpeed,
                                                            float DeltaTime)
{
	float LocalLR = FMath::FInterpTo(Current.LR, Target.LR, DeltaTime, InterpSpeed);
	float LocalFB = FMath::FInterpTo(Current.FB, Target.FB, DeltaTime, InterpSpeed);
	return FALSLeanAmount(LocalLR, LocalFB);
}

float UBaseCharacterAnimInstance::GetAnimCurveCompact(FName CurveName) const
{
	return GetCurveValue(CurveName);
}

float UBaseCharacterAnimInstance::GetAnimCurveClamped(FName CurveName, float Bias, float ClampMin, float ClampMax) const
{
	return FMath::Clamp(GetCurveValue(CurveName) + Bias, ClampMin, ClampMax);
}

void UBaseCharacterAnimInstance::PlayDynamicTransitionDelay()
{
	bCanPlayDynamicTransition = true;
}

void UBaseCharacterAnimInstance::OnJumpedDelay()
{
	InAir.bJumped = false;
}

void UBaseCharacterAnimInstance::OnPivotDelay()
{
	Grounded.bPivot = false;
}
