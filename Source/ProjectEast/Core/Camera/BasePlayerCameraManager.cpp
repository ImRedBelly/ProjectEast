#include "BasePlayerCameraManager.h"
#include "PlayerCameraAnimInstance.h"
#include "ProjectEast/Core/Characters/Interfaces/CameraParameters.h"

void ABasePlayerCameraManager::OnPossess(APawn* NewPawn)
{
	ControlledPawn = NewPawn;

	if (UPlayerCameraAnimInstance* PlayerCameraAnimInstance = Cast<UPlayerCameraAnimInstance>(
		CameraBehavior->GetAnimInstance()))
		PlayerCameraAnimInstance->InitializeAnimInstance(ControlledPawn);
}

ABasePlayerCameraManager::ABasePlayerCameraManager()
{
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>("CameraBehavior");
}


FCameraInfo ABasePlayerCameraManager::CustomCameraBehavior()
{
	if (ICameraParameters* CameraParameters = Cast<ICameraParameters>(ControlledPawn))
	{
		//Step 1
		FTransform PivotTarget = CameraParameters->GetTPPivotTarget();
		FVector FPTarget = CameraParameters->GetFPCameraTarget();

		auto Parameters = CameraParameters->GetCameraParameters();
		float TPFOV = Parameters.TPFOV;
		float FPFOV = Parameters.FPFOV;

		//Step 2
		FRotator CurrentCameraRotation = GetCameraRotation();
		FRotator ControlRotation = GetOwningPlayerController()->GetControlRotation();

		float DeltaTime = GetWorld()->GetDeltaSeconds();

		float RotationLagSpeed = GetCameraBehaviorParam(CurveName_RotationLagSpeed);
		FRotator LocalTargetCameraRotation = FMath::RInterpTo(CurrentCameraRotation, ControlRotation, DeltaTime,
		                                                      RotationLagSpeed);

		float OverrideDebug = GetCameraBehaviorParam(CurveName_OverrideDebug);
		FRotator TargetCameraRotation = FMath::Lerp(LocalTargetCameraRotation, DebugViewRotation, OverrideDebug);


		//Step 3
		auto SpeedX = GetCameraBehaviorParam(CurveName_PivotLagSpeedX);
		auto SpeedY = GetCameraBehaviorParam(CurveName_PivotLagSpeedY);
		auto SpeedZ = GetCameraBehaviorParam(CurveName_PivotLagSpeedZ);
		FVector Position = CalculateAxisIndependentLag(SmoothedPivotTarget.GetLocation(), PivotTarget.GetLocation(),
		                                               TargetCameraRotation, FVector(SpeedX, SpeedY, SpeedZ), DeltaTime);
		SmoothedPivotTarget = FTransform(PivotTarget.GetRotation(), Position, FVector::OneVector);

		//Step 4
		FRotator SmoothedRotator = SmoothedPivotTarget.GetRotation().Rotator();
		FVector SmoothedForwardVector = FRotationMatrix(SmoothedRotator).GetUnitAxis(EAxis::X);
		FVector SmoothedRightVector = FRotationMatrix(SmoothedRotator).GetUnitAxis(EAxis::Y);
		FVector SmoothedUpVector = FRotationMatrix(SmoothedRotator).GetUnitAxis(EAxis::Z);

		FVector PivotLocation = SmoothedPivotTarget.GetLocation() +
			(SmoothedForwardVector * GetCameraBehaviorParam(CurveName_PivotOffsetX)) +
			(SmoothedRightVector * GetCameraBehaviorParam(CurveName_PivotOffsetY)) +
			(SmoothedUpVector * GetCameraBehaviorParam(CurveName_PivotOffsetZ));

		//Step 5
		FVector ForwardVector = FRotationMatrix(TargetCameraRotation).GetUnitAxis(EAxis::X);
		FVector RightVector = FRotationMatrix(TargetCameraRotation).GetUnitAxis(EAxis::Y);
		FVector UpVector = FRotationMatrix(TargetCameraRotation).GetUnitAxis(EAxis::Z);


		auto FutureTargetCameraLocation = PivotLocation +
			(ForwardVector * GetCameraBehaviorParam(CurveName_CameraOffsetX)) +
			(RightVector * GetCameraBehaviorParam(CurveName_CameraOffsetY)) +
			(UpVector * GetCameraBehaviorParam(CurveName_CameraOffsetZ));

		FVector TargetCameraLocation = FMath::Lerp(FutureTargetCameraLocation, PivotLocation + DebugViewOffset,
		                                   GetCameraBehaviorParam(CurveName_OverrideDebug));

		//Step 6
		// auto TraceParams = CameraParameters->GetTPTraceParams();
		//
		// FCollisionQueryParams CollisionParams(FName(TEXT("SphereTrace")), false, this);
		// CollisionParams.bTraceComplex = true;
		// CollisionParams.bReturnPhysicalMaterial = false;
		//
		// FHitResult HitResult;
		//
		// GetWorld()->SweepSingleByChannel(
		// 	HitResult,
		// 	TraceParams.TraceOrigin,
		// 	TargetCameraLocation,
		// 	FQuat::Identity,
		// 	TraceParams.TraceChannel,
		// 	FCollisionShape::MakeSphere(TraceParams.TraceRadius),
		// 	CollisionParams);
		//
		// if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
		// 	TargetCameraLocation = TargetCameraLocation - (HitResult.Location - HitResult.TraceEnd);
		//

		//Step 7
		//Draw Debug


		//Step 8

		FVector ValueLocation = FMath::Lerp(TargetCameraLocation, FPTarget,
		                                    GetCameraBehaviorParam(CurveName_WeightFirstPerson));
		FVector FinalLocation = FMath::Lerp(ValueLocation, TargetCameraLocation,
		                                    GetCameraBehaviorParam(CurveName_OverrideDebug));
		FRotator FinalRotation = FMath::Lerp(TargetCameraRotation, DebugViewRotation,
		                                     GetCameraBehaviorParam(CurveName_OverrideDebug));

		CameraInfo.Location = FinalLocation;
		CameraInfo.Rotation = FinalRotation;
		CameraInfo.FOV = FMath::Lerp(TPFOV, FPFOV, GetCameraBehaviorParam(CurveName_WeightFirstPerson));
	}

	return CameraInfo;
}

FVector ABasePlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation,
                                                              FRotator CameraRotation, FVector LagSpeeds,
                                                              float DeltaTime) const
{
	FVector UnrotatedTargetLocation = FRotationMatrix(CameraRotation).InverseTransformVector(TargetLocation);
	FVector UnrotatedCurrentLocation = FRotationMatrix(CameraRotation).InverseTransformVector(CurrentLocation);

	float InterpolatedX = FMath::FInterpTo(UnrotatedCurrentLocation.X, UnrotatedTargetLocation.X, DeltaTime,LagSpeeds.X);
	float InterpolatedY = FMath::FInterpTo(UnrotatedCurrentLocation.Y, UnrotatedTargetLocation.Y, DeltaTime,LagSpeeds.Y);
	float InterpolatedZ = FMath::FInterpTo(UnrotatedCurrentLocation.Z, UnrotatedTargetLocation.Z, DeltaTime,LagSpeeds.Z);

	FVector InterpolatedVector(InterpolatedX, InterpolatedY, InterpolatedZ);
	FVector FinalLocation = FRotationMatrix(CameraRotation).TransformVector(InterpolatedVector);

	return FinalLocation;
}

float ABasePlayerCameraManager::GetCameraBehaviorParam(FName CurveName) const
{
	if (auto AnimInstance = CameraBehavior->GetAnimInstance())
		return AnimInstance->GetCurveValue(CurveName);
	return 0.0f;
}
