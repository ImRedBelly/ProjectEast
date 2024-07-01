#include "WallRunComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UWallRunComponent::UWallRunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner() -> IsA<ACharacter>(),
	       TEXT("ULedgeDetectorComponent::BeginPlay() only a character can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());

	GatherCharacterInformation();

	PrimaryTraceSize = CapsuleRadius * 0.85f;
	CachedCharacterOwner->MovementModeChangedDelegate.AddDynamic(this, &UWallRunComponent::OnCharacterMovement);

	UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
	if (MovementComponent->MovementMode == MOVE_Falling)
	{
		SetComponentTickEnabled(true);
	}
	CachedCharacterOwner->OnActorHit.AddDynamic(this, &UWallRunComponent::OnActorHit);
}

void UWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsWallRunning)
	{
		DetectWall(WallSide);
	}
	else
	{
		WallJumpTick(DeltaTime);
		DetectWall(EDirectionType::Right);

		if (!bIsWallRunning)
			DetectWall(EDirectionType::Left);
	}

	WallRunTick(DeltaTime);
	PreviousVelocity = CachedCharacterOwner->GetVelocity();
}

bool UWallRunComponent::StartWallJump()
{
	if (bIsWallRunning)
	{
		StopWallRun(MOVE_Falling, 0.1f);

		float WallSideValue = WallSide == EDirectionType::Right ? 90 : -90;
		float RangeValue = UKismetMathLibrary::MapRangeClamped(WallJumpAngle, 90.0f, 0.0f, 0.0f, WallSideValue);


		FVector DirectionLaunchCharacter = FRotator(0.0f, RangeValue, 0.0f).RotateVector(AverageWallNormal);
		DirectionLaunchCharacter *= WallJumpHorizontalVelocity;
		DirectionLaunchCharacter.Z = WallJumpVerticalVelocity;

		CachedCharacterOwner->LaunchCharacter(DirectionLaunchCharacter, true, true);

		bIsWallJumping = true;

		UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
		MovementComponent->bOrientRotationToMovement = false;
		MovementComponent->AirControl = 0.0f;
		DelayedAirControl();

		return true;
	}
	return false;
}

void UWallRunComponent::StopWallJump()
{
	bIsWallJumping = false;
	UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = DefaultOrientRotation;
	MovementComponent->AirControl = DefaultAirControl;
	MovementComponent->MaxAcceleration = DefaultMaxAcceleration;

	if (GetWorld()->GetTimerManager().IsTimerActive(WallJumpAirControlTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(WallJumpAirControlTimer);
		WallJumpAirControlTimer.Invalidate();
	}

	UpdateRotationALS();
}

bool UWallRunComponent::StartWallRun(EDirectionType Direction)
{
	if (WallRunningAvailable)
	{
		FVector PreviousVelocityNormalize = PreviousVelocity.GetSafeNormal(0.0001f);
		float DotProduct1 = UKismetMathLibrary::Dot_VectorVector(PreviousVelocityNormalize, AverageWallNormal);

		UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
		FVector LastInputVector = MovementComponent->GetLastInputVector();
		float DotProduct2 = UKismetMathLibrary::Dot_VectorVector(LastInputVector, AverageWallNormal);


		if (DotProduct1 < 0.0f || DotProduct2 < 0.0f)
		{
			if (bIsWallJumping)
			{
				StopWallJump();
			}

			bIsWallRunning = true;
			WallSide = Direction;

			MovementComponent->MaxFlySpeed = WallRunSpeed;
			MovementComponent->bOrientRotationToMovement = false;
			MovementComponent->MaxAcceleration = WallRunAcceleration;
			MovementComponent->BrakingDecelerationFlying = WallRunBrakingDeceleration;
			MovementComponent->SetMovementMode(MOVE_Flying, 0);
			CurrentWallRunDuration = 0;
			PreviousRotation = CachedCharacterOwner->GetActorRotation();

			if (WallRunDurationLimit >= 0)
			{
				StartDurationCountdown();
			}

			StartMonitoringSpeed();
			MovementComponent->Velocity = FVector(0, 0, 0);
			return true;
		}
	}

	return false;
}

void UWallRunComponent::StopWallRun(EMovementMode NewMovementMode, float TemporarilyDisableWallRunSeconds)
{
	bIsWallRunning = false;

	UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
	MovementComponent->MaxFlySpeed = DefaultMaxFlySpeed;
	MovementComponent->bOrientRotationToMovement = DefaultOrientRotation;
	MovementComponent->MaxAcceleration = DefaultMaxAcceleration;
	MovementComponent->BrakingDecelerationFlying = DefaultBrakingDeceleration;

	MovementComponent->SetMovementMode(NewMovementMode);
	GetWorld()->GetTimerManager().ClearTimer(DurationCountdownTimer);
	DurationCountdownTimer.Invalidate();

	StopMonitoringSpeed();

	if (TemporarilyDisableWallRunSeconds > 0)
	{
		TemporarilyDisableWallRunning(TemporarilyDisableWallRunSeconds);
	}
	UpdateRotationALS();
}


void UWallRunComponent::WallRunTick(float DeltaSeconds)
{
	if (bIsWallRunning)
	{
		HugWall();

		FVector CrossProduct = UKismetMathLibrary::Cross_VectorVector(AverageWallNormal,
		                                                              FVector(0, 0, WallSide == EDirectionType::Right
			                                                                      ? -1
			                                                                      : 1));
		float RangeValue = UKismetMathLibrary::MapRangeClamped(CurrentWallRunDuration, 0.0f, 2.5f, 1.0f, -2.0f);

		FVector WorldDirection = FVector(CrossProduct.X, CrossProduct.Y, RangeValue * ArcAmount);
		CachedCharacterOwner->AddMovementInput(WorldDirection, 10000.0f, false);


		FRotator CurrentRotation = CachedCharacterOwner->GetActorRotation();

		float CurrentVelocityLength = CachedCharacterOwner->GetVelocity().Size();
		float RangeValueRotation = UKismetMathLibrary::MapRangeClamped(CurrentVelocityLength, 800.0f, 1600.0f, 8.0f,
		                                                               16.0f);

		FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CrossProduct);
		FRotator FutureRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds,
		                                                        RangeValueRotation);
		CachedCharacterOwner->SetActorRotation(FutureRotation);

		CurrentWallRunDuration += DeltaSeconds;
		TopHeightCheck();
		BottomHeightCheck(WallSide);

		UpdateAnimationsValue(DeltaSeconds);
	}
}

void UWallRunComponent::WallJumpTick(float DeltaSeconds) const
{
	if (bIsWallJumping)
	{
		FRotator CurrentRotation = CachedCharacterOwner->GetActorRotation();
		FVector Velocity = CachedCharacterOwner->GetVelocity();
		FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, FRotator(0, TargetRotation.Yaw, 0), DeltaSeconds,
		                                        5.0f);
		CachedCharacterOwner->SetActorRotation(NewRotation);
	}
}

void UWallRunComponent::GatherCharacterInformation()
{
	CapsuleRadius = CachedCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();

	UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
	DefaultMaxFlySpeed = MovementComponent->MaxFlySpeed;
	DefaultOrientRotation = MovementComponent->bOrientRotationToMovement;
	DefaultMaxAcceleration = MovementComponent->MaxAcceleration;
	DefaultBrakingDeceleration = MovementComponent->BrakingDecelerationFlying;
	DefaultAirControl = MovementComponent->AirControl;
}


void UWallRunComponent::DetectWall(EDirectionType DirectionType)
{
	FVector ActorLocation = CachedCharacterOwner->GetActorLocation();
	FVector RightVector = CachedCharacterOwner->GetActorRightVector();
	FVector ForwardVector = CachedCharacterOwner->GetActorForwardVector();


	float RunningValue = bIsWallRunning ? 2.0f : 0.3f;
	float SideValue = DirectionType == EDirectionType::Left ? -1.0f : 1.0f;


	float LengthTrace = (RunningValue * CapsuleRadius) * SideValue;

	FVector StartLocation = ActorLocation + (ForwardVector * (CapsuleRadius * 0.05f));
	FVector EndLocation = StartLocation + (RightVector * LengthTrace);

	FHitResult Result1;

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, PrimaryTraceSize,
	                                        UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
	                                        TArray<AActor*>(), EDrawDebugTrace::None, Result1, true);

	if (Result1.bBlockingHit)
	{
		if (IsWallRunnable(Result1.GetActor()))
		{
			PrimaryTraceNormal = Result1.ImpactNormal;
			AverageWallNormal = PrimaryTraceNormal;

			CurrentWallLocation = Result1.ImpactPoint;
			DistanceFromWall = (Result1.TraceStart - Result1.ImpactPoint).Length() - CapsuleRadius;

			FHitResult Result2;
			Result2 = LineTraceRelativeToCapsuleAndWall(PrimaryTraceNormal, CurrentWallLocation,
			                                            1.5f, -0.9f, 0.75f,
			                                            -1.5f, 0.0f,
			                                            DirectionType, true);


			if (Result2.bBlockingHit)
			{
				if (IsWallRunnable(Result2.GetActor()))
				{
					if (bIsWallRunning)
					{
						bool bIsEndWallRun = CheckCornerAngle(PrimaryTraceNormal, Result2.ImpactNormal);
						if (bIsEndWallRun)
						{
							SecondaryTraceNormal = Result2.ImpactNormal;
							FVector NewAverageWallNormal = (SecondaryTraceNormal + AverageWallNormal).GetSafeNormal(
								0.0001f);
							AverageWallNormal = NewAverageWallNormal;
							CheckEnoughVerticalSpace(DirectionType);
						}
						else
						{
							FHitResult Result3;
							Result3 = LineTraceRelativeToCapsuleAndWall(PrimaryTraceNormal, CurrentWallLocation, 1.5f,
							                                            -1.0f,
							                                            0.75f, -1.8f, 0.0f, DirectionType, true);

							if (Result3.bBlockingHit)
							{
								if (IsWallRunnable(Result3.GetActor()))
								{
									bIsEndWallRun = CheckCornerAngle(PrimaryTraceNormal, Result3.ImpactNormal);
									if (bIsEndWallRun)
									{
										SecondaryTraceNormal = Result3.ImpactNormal;
										FVector NewAverageWallNormal = (SecondaryTraceNormal + AverageWallNormal).
											GetSafeNormal(0.0001f);
										AverageWallNormal = NewAverageWallNormal;
										CheckEnoughVerticalSpace(DirectionType);
									}
									else
									{
										StopWallRun(MOVE_Falling, 1.0f);
									}
								}
								else
								{
									StopWallRun(MOVE_Falling, 1.0f);
								}
							}
							else
							{
								StopWallRun(MOVE_Falling, 1.0f);
							}
						}
					}
					else
					{
						SecondaryTraceNormal = Result2.ImpactNormal;
						FVector NewAverageWallNormal = (SecondaryTraceNormal + AverageWallNormal).
							GetSafeNormal(0.0001f);
						AverageWallNormal = NewAverageWallNormal;

						CheckEnoughVerticalSpace(DirectionType);
					}
				}
			}
			else
			{
				FHitResult Result4;
				Result4 = LineTraceRelativeToCapsuleAndWall(PrimaryTraceNormal, CurrentWallLocation, 0.0f, -1.5f,
				                                            0.0f, 2.0f, -0.3f, DirectionType, true);
				if (Result4.bBlockingHit)
				{
					if (IsWallRunnable(Result4.GetActor()))
					{
						if (bIsWallRunning)
						{
							bool bIsEndWall = CheckCornerAngle(PrimaryTraceNormal, Result4.ImpactNormal);

							if (bIsEndWall)
							{
								SecondaryTraceNormal = Result4.ImpactNormal;
								FVector NewAverageWallNormal = (SecondaryTraceNormal + AverageWallNormal).GetSafeNormal(
									0.0001f);
								AverageWallNormal = NewAverageWallNormal;

								CheckEnoughVerticalSpace(DirectionType);
							}
							else
							{
								StopWallRun(MOVE_Falling, 1.0f);
							}
						}
					}
				}
			}
		}
		else
		{
			if (bIsWallRunning)
			{
				StopWallRun(MOVE_Falling, 0.0f);
			}
		}
	}
	else
	{
		if (bIsWallRunning)
		{
			StopWallRun(MOVE_Falling, 0.0f);
		}
	}
}

bool UWallRunComponent::IsWallRunnable(const AActor* WallActor) const
{
	if (!IsValid(WallActor))
		return !RequiresTag;

	if (WallActor->ActorHasTag(TagForPreventingWallRun))
		return false;

	if (RequiresTag)
	{
		if (WallActor->ActorHasTag(TagForAllowingWallRun))
		{
			return true;
		}
		return false;
	}
	return true;
}

FHitResult UWallRunComponent::LineTraceRelativeToCapsuleAndWall(FVector WallNormal, FVector WallLocation,
                                                                float InitialTraceLength,
                                                                float StartOffset_Backwards,
                                                                float StartOffset_AwayFromWall,
                                                                float EndOffset_Backwards,
                                                                float EndOffset_AwayFromWall,
                                                                EDirectionType DirectionType, bool ShowDebugLines) const
{
	FVector ForwardDirection = UKismetMathLibrary::Cross_VectorVector(WallNormal,
	                                                                  FVector(
		                                                                  0, 0, DirectionType == EDirectionType::Right
			                                                                  ? 1
			                                                                  : -1));

	float StartPositionForwardsOffset = (StartOffset_Backwards * CapsuleRadius);
	FVector StartPositionSidewaysOffset = (StartOffset_AwayFromWall * CapsuleRadius) * WallNormal;

	FVector CombiningFinalizingStartPosition = WallLocation + (ForwardDirection * StartPositionForwardsOffset) +
		StartPositionSidewaysOffset;

	FVector TraceLengthBeforeAnyEndPositionOffsetsApplied = WallNormal * (CapsuleRadius * InitialTraceLength * -1);
	FVector EndPositionForwardsOffset = ForwardDirection * (CapsuleRadius * EndOffset_Backwards);
	FVector EndPositionSidewaysOffset = WallNormal * (CapsuleRadius * EndOffset_AwayFromWall);

	FVector CombiningFinalizingEndPosition = CombiningFinalizingStartPosition +
		TraceLengthBeforeAnyEndPositionOffsetsApplied +
		EndPositionForwardsOffset + EndPositionSidewaysOffset;

	FHitResult Result;

	UKismetSystemLibrary::LineTraceSingle(GetWorld(),
	                                      CombiningFinalizingStartPosition, CombiningFinalizingEndPosition,
	                                      UEngineTypes::ConvertToTraceType(ECC_Visibility),
	                                      false, TArray<AActor*>(),
	                                      ShowDebugLines ? EDrawDebugTrace::None : EDrawDebugTrace::None, Result,
	                                      true, FColor::Yellow);

	return Result;
}

bool UWallRunComponent::CheckCornerAngle(FVector FirstWallNormal, FVector SecondWallNormal)
{
	FVector2D FirstWallNormal2D = UKismetMathLibrary::Conv_VectorToVector2D(FirstWallNormal);
	FVector2D SecondWallNormal2D = UKismetMathLibrary::Conv_VectorToVector2D(SecondWallNormal);
	float DotProduct = UKismetMathLibrary::DotProduct2D(FirstWallNormal2D, SecondWallNormal2D);

	float Acos = UKismetMathLibrary::DegAcos(DotProduct);
	CurrentCornerAngle = UKismetMathLibrary::FTrunc64(Acos);

	FVector Cross = UKismetMathLibrary::Cross_VectorVector(FirstWallNormal, SecondWallNormal);
	float Sign = UKismetMathLibrary::SignOfFloat(Cross.Z);

	bool bIsOutsideRight = WallSide == EDirectionType::Right && Sign >= 0.0f;
	bool bIsOutsideLeft = WallSide == EDirectionType::Left && Sign < 0.0f;

	if (bIsOutsideRight || bIsOutsideLeft)
		return CurrentCornerAngle <= OutsideCornerAngleLimit;

	return CurrentCornerAngle <= InsideCornerAngleLimit;
}

void UWallRunComponent::CheckEnoughVerticalSpace(EDirectionType DirectionType)
{
	if (!bIsWallRunning)
	{
		if (BottomHeightCheck(DirectionType) && TopHeightCheck())
		{
			StartWallRun(DirectionType);
		}
	}
}

bool UWallRunComponent::BottomHeightCheck(EDirectionType DirectionType)
{
	FVector ActorLocation = CachedCharacterOwner->GetActorLocation();
	float ScaledCapsuleHalfHeight = CachedCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FVector StartLocation = ActorLocation - FVector(0, 0, ScaledCapsuleHalfHeight);

	FVector ActorRightVector = CachedCharacterOwner->GetActorRightVector();
	float SideValue = DirectionType == EDirectionType::Left ? -1 : 1;

	float LengthTrace = CapsuleRadius * 2.0f;
	FVector EndLocation = StartLocation + ((ActorRightVector * SideValue) * LengthTrace);

	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, CapsuleRadius * 0.6f,
	                                        UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
	                                        TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);

	if (HitResult.bBlockingHit)
		return true;
	else
	{
		if (bIsWallRunning)
		{
			StopWallRun(MOVE_Falling, 0.0f);
		}
		return false;
	}
}

bool UWallRunComponent::TopHeightCheck()
{
	FVector ActorLocation = CachedCharacterOwner->GetActorLocation();

	float ScaledCapsuleHalfHeight = CachedCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ScaledCapsuleHalfHeight *= 0.65f;

	FVector StartLocation = ActorLocation + FVector(0, 0, ScaledCapsuleHalfHeight);
	FVector EndLocation = StartLocation + (AverageWallNormal * (CapsuleRadius * -1.7f));


	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation,
	                                      UEngineTypes::ConvertToTraceType(ECC_Visibility),
	                                      false, TArray<AActor*>(), EDrawDebugTrace::None,
	                                      HitResult, true, FColor::Magenta);

	if (HitResult.bBlockingHit)
	{
		if (IsWallRunnable(HitResult.GetActor()))
			return SetLastKnownSafeHeight();

		return BackupCapsuleTrace(StartLocation);
	}
	else
	{
		return BackupCapsuleTrace(StartLocation);
	}
}

bool UWallRunComponent::SetLastKnownSafeHeight()
{
	LastKnownSafeHeight = CachedCharacterOwner->GetActorLocation().Z;
	return true;
}

bool UWallRunComponent::SetLastRecordedSafeHeight() const
{
	if (bIsWallRunning)
	{
		FVector ActorLocation = CachedCharacterOwner->GetActorLocation();
		ActorLocation.Z = LastKnownSafeHeight;
		CachedCharacterOwner->SetActorLocation(ActorLocation, true);
	}

	return false;
}

bool UWallRunComponent::SetDesiredMaxHeightForCurrentWall()
{
	FVector ActorLocation = CachedCharacterOwner->GetActorLocation();
	ActorLocation.Z -= DistanceAboveWall;
	LastKnownSafeHeight = ActorLocation.Z;
	CachedCharacterOwner->SetActorLocation(ActorLocation, true);

	return true;
}

void UWallRunComponent::HugWall() const
{
	FVector DeltaLocation = AverageWallNormal * (1 - DistanceFromWall);
	CachedCharacterOwner->AddActorWorldOffset(DeltaLocation, true);
}

bool UWallRunComponent::BackupCapsuleTrace(FVector StartLocation)
{
	FVector EndLocation = StartLocation + (AverageWallNormal * (CapsuleRadius * -1.1f));

	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, CapsuleRadius * 0.6f,
	                                        UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
	                                        TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true,
	                                        FColor::Green);

	if (HitResult.bBlockingHit)
	{
		if (IsWallRunnable(HitResult.GetActor()))
		{
			if (StartLocation.Z > HitResult.ImpactPoint.Z)
			{
				if (bIsWallRunning)
				{
					DistanceAboveWall = StartLocation.Z - HitResult.ImpactPoint.Z;
					return SetDesiredMaxHeightForCurrentWall();
				}
				else
				{
					return SetLastRecordedSafeHeight();
				}
			}
			else
			{
				return SetLastKnownSafeHeight();
			}
		}
		else
		{
			return SetLastRecordedSafeHeight();
		}
	}
	else
	{
		if (bIsWallRunning)
		{
			FVector NewStartLocation = StartLocation - (AverageWallNormal * (CapsuleRadius * 0.5f));
			FVector NewEndLocation(0, 0, NewStartLocation.Z - (CapsuleRadius * 1.5f));

			UKismetSystemLibrary::SphereTraceSingle(GetWorld(), NewStartLocation, NewEndLocation, CapsuleRadius * 0.6f,
			                                        UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
			                                        TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true,
			                                        FColor::Green);

			if (HitResult.bBlockingHit)
			{
				DistanceAboveWall = StartLocation.Z - HitResult.ImpactPoint.Z;
				return SetDesiredMaxHeightForCurrentWall();
			}
			else
			{
				return SetLastRecordedSafeHeight();
			}
		}
		else
		{
			return false;
		}
	}
}

void UWallRunComponent::UpdateAnimationsValue(float DeltaSeconds)
{
	FVector Velocity = CachedCharacterOwner->GetVelocity().GetSafeNormal(0.0001f);
	float DotProduct = UKismetMathLibrary::Dot_VectorVector(Velocity, FVector(Velocity.X, Velocity.Y, 0));
	CurrentArcAngle = UKismetMathLibrary::DegAcos(DotProduct) * Velocity.Z;

	FRotator CurrentRotation = CachedCharacterOwner->GetActorRotation();
	FRotator FutureRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, PreviousRotation);
	FVector TargetRotation = FutureRotation.Vector() / DeltaSeconds;

	float RangeValue = UKismetMathLibrary::MapRangeClamped(TargetRotation.Y, -10, 10.0f, -1.0f, 1.0f)
		* (WallSide == EDirectionType::Right ? 1 : -1);

	float Value = UKismetMathLibrary::Max(RangeValue, 0.0f);

	CurrentTurnRate = UKismetMathLibrary::FInterpTo(CurrentTurnRate, Value, DeltaSeconds, 10.0f);
	PreviousRotation = CurrentRotation;
}

void UWallRunComponent::DelayedAirControl()
{
	GetWorld()->GetTimerManager().ClearTimer(WallJumpAirControlTimer);
	GetWorld()->GetTimerManager().SetTimer(WallJumpAirControlTimer, this, &UWallRunComponent::EnableWallJumpAirControl,
	                                       0.35f, false);
}

void UWallRunComponent::EnableWallJumpAirControl() const
{
	UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
	MovementComponent->AirControl = WallJumpAirControl;

	float RangeValue = UKismetMathLibrary::MapRangeClamped(WallJumpAirControl, 0.0f, 1.0f, 2048.0f, 6000.0f);

	MovementComponent->MaxAcceleration = RangeValue;
}

void UWallRunComponent::StopMonitoringSpeed()
{
	GetWorld()->GetTimerManager().ClearTimer(SpeedMonitorTimer);
	SpeedMonitorTimer.Invalidate();
}

void UWallRunComponent::TemporarilyDisableWallRunning(float DisableTime)
{
	WallRunningAvailable = false;

	GetWorld()->GetTimerManager().ClearTimer(DisabledWallRunTimer);
	GetWorld()->GetTimerManager().SetTimer(DisabledWallRunTimer, this, &UWallRunComponent::AllowWallRunning,
	                                       DisableTime, false);
}

void UWallRunComponent::UpdateRotationALS()
{
	//TODO Create rotation player in air
}

void UWallRunComponent::StartDurationCountdown()
{
	GetWorld()->GetTimerManager().ClearTimer(DurationCountdownTimer);
	GetWorld()->GetTimerManager().SetTimer(DurationCountdownTimer, this, &UWallRunComponent::WallRunTimeout,
	                                       WallRunDurationLimit, false);
}

void UWallRunComponent::StartMonitoringSpeed()
{
	bSpeedTooLow = false;
	GetWorld()->GetTimerManager().ClearTimer(SpeedMonitorTimer);
	GetWorld()->GetTimerManager().SetTimer(SpeedMonitorTimer, this, &UWallRunComponent::MonitorSpeed,
	                                       0.2f, true);
}

void UWallRunComponent::AllowWallRunning()
{
	WallRunningAvailable = true;
}

void UWallRunComponent::MonitorSpeed()
{
	FVector Velocity = CachedCharacterOwner->GetVelocity();

	if (FVector(Velocity.X, Velocity.Y, 0).Length() < WallRunSpeed * 0.25f)
	{
		if (bSpeedTooLow)
		{
			StopWallRun(MOVE_Falling, 1.0f);
		}
		else
		{
			bSpeedTooLow = true;
			OpenGate();
			CloseGate();
		}
	}
	else
	{
		EnterGate();
	}
}

void UWallRunComponent::WallRunTimeout()
{
	StopWallRun(MOVE_Falling, 2.0f);
}

void UWallRunComponent::OnCharacterMovement(ACharacter* Character, EMovementMode PrevMovementMode,
                                            uint8 PreviousCustomMode)
{
	UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
	switch (MovementComponent->MovementMode)
	{
	case MOVE_Falling:
		SetComponentTickEnabled(true);
		PreviousVelocity = CachedCharacterOwner->GetVelocity();
		break;
	case MOVE_Flying:
		if (!bIsWallRunning)
		{
			if (IsComponentTickEnabled())
			{
				SetComponentTickEnabled(false);
			}
			if (bIsWallJumping)
			{
				StopWallJump();
			}
		}
		break;
	case MOVE_None:
	case MOVE_Walking:
	case MOVE_NavWalking:
	case MOVE_Swimming:
	case MOVE_Custom:
		if (IsComponentTickEnabled())
		{
			SetComponentTickEnabled(false);
		}
		if (bIsWallJumping)
		{
			StopWallJump();
		}

		break;
	}
}

void UWallRunComponent::OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	UCharacterMovementComponent* MovementComponent = CachedCharacterOwner->GetCharacterMovement();
	if (MovementComponent->IsWalkable(Hit))
	{
		if (bIsWallRunning)
		{
			FVector ActorLocation = CachedCharacterOwner->GetActorLocation();

			float TargetPosition = ActorLocation.Z - CachedCharacterOwner->GetCapsuleComponent()->
			                                                               GetScaledCapsuleHalfHeight_WithoutHemisphere();

			if (Hit.ImpactPoint.Z <= TargetPosition)
			{
				StopWallRun(MOVE_Walking, 0.0f);
			}
		}
		else
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(DisabledWallRunTimer))
			{
				GetWorld()->GetTimerManager().ClearTimer(DisabledWallRunTimer);
				DisabledWallRunTimer.Invalidate();

				AllowWallRunning();
			}
		}
	}
}

void UWallRunComponent::EnterGate()
{
	if (bGateOpen)
	{
		bSpeedTooLow = false;
	}
}

void UWallRunComponent::OpenGate()
{
	bGateOpen = true;
	EnterGate();
}

void UWallRunComponent::CloseGate()
{
	bGateOpen = false;
	EnterGate();
}
