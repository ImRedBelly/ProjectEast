#include "BasePlayerCameraManager.h"
#include "PlayerCameraAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
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
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (ICameraParameters* CameraParameters = Cast<ICameraParameters>(ControlledPawn))
	{
		// Step 1: Get Camera Parameters from CharacterBP via the Camera Interface

		FTransform PivotTarget = CameraParameters->GetTPPivotTarget();
		FVector FPTarget = CameraParameters->GetFPCameraTarget();

		auto Parameters = CameraParameters->GetCameraParameters();
		float TPFOV = Parameters.TPFOV;
		float FPFOV = Parameters.FPFOV;
		
		// Step 2: Calculate Target Camera Rotation. Use the Control Rotation and interpolate for smooth camera rotation.
		const FRotator& InterpResult = FMath::RInterpTo(GetCameraRotation(),
		                                                GetOwningPlayerController()->GetControlRotation(), DeltaTime,
		                                                GetCameraBehaviorParam(CurveName_RotationLagSpeed));

		FRotator TargetCameraRotation = UKismetMathLibrary::RLerp(InterpResult, DebugViewRotation,
		                                                          GetCameraBehaviorParam(CurveName_OverrideDebug), true);


		// Step 3: Calculate the Smoothed Pivot Target (Orange Sphere).
		// Get the 3P Pivot Target (Green Sphere) and interpolate using axis independent lag for maximum control.
		const FVector LagSpd(GetCameraBehaviorParam(CurveName_PivotLagSpeedX),
		                     GetCameraBehaviorParam(CurveName_PivotLagSpeedY),
		                     GetCameraBehaviorParam(CurveName_PivotLagSpeedZ));

		const FVector& AxisIndpLag = CalculateAxisIndependentLag(SmoothedPivotTarget.GetLocation(),
		                                                         PivotTarget.GetLocation(), TargetCameraRotation,
		                                                         LagSpd,
		                                                         DeltaTime);

		SmoothedPivotTarget.SetRotation(PivotTarget.GetRotation());
		SmoothedPivotTarget.SetLocation(AxisIndpLag);
		SmoothedPivotTarget.SetScale3D(FVector::OneVector);

		// Step 4: Calculate Pivot Location (BlueSphere). Get the Smoothed
		// Pivot Target and apply local offsets for further camera control.
		FVector PivotLocation = SmoothedPivotTarget.GetLocation() +
			UKismetMathLibrary::GetForwardVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(CurveName_PivotOffsetX) +
			UKismetMathLibrary::GetRightVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(CurveName_PivotOffsetY) +
			UKismetMathLibrary::GetUpVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(CurveName_PivotOffsetZ);

		// Step 5: Calculate Target Camera Location. Get the Pivot location and apply camera relative offsets.
		FVector TargetCameraLocation = UKismetMathLibrary::VLerp(
			PivotLocation +
			UKismetMathLibrary::GetForwardVector(TargetCameraRotation) * GetCameraBehaviorParam(CurveName_CameraOffsetX) +
			UKismetMathLibrary::GetRightVector(TargetCameraRotation) * GetCameraBehaviorParam(CurveName_CameraOffsetY) +
			UKismetMathLibrary::GetUpVector(TargetCameraRotation) * GetCameraBehaviorParam(CurveName_CameraOffsetZ),

			PivotTarget.GetLocation() + DebugViewOffset,GetCameraBehaviorParam(CurveName_OverrideDebug));

	
		auto TraceParams = CameraParameters->GetTPTraceParams();
		ECollisionChannel TraceChannel = TraceParams.TraceChannel;
		float TraceRadius = TraceParams.TraceRadius;
		FVector TraceOrigin = TraceParams.TraceOrigin;

		UWorld* World = GetWorld();
		check(World);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(ControlledPawn);

		FHitResult HitResult;
		const FCollisionShape SphereCollisionShape = FCollisionShape::MakeSphere(TraceRadius);
		const bool bHit = World->SweepSingleByChannel(HitResult, TraceOrigin, TargetCameraLocation, FQuat::Identity,
		                                              TraceChannel, SphereCollisionShape, Params);

		// if (ALSDebugComponent&& ALSDebugComponent->GetShowTraces())
		// {
		// 	UALSDebugComponent::DrawDebugSphereTraceSingle(World,
		// 	                                               TraceOrigin,
		// 	                                               TargetCameraLocation,
		// 	                                               SphereCollisionShape,
		// 	                                               EDrawDebugTrace::Type::ForOneFrame,
		// 	                                               bHit,
		// 	                                               HitResult,
		// 	                                               FLinearColor::Red,
		// 	                                               FLinearColor::Green,
		// 	                                               5.0f);
		// }

		if (HitResult.IsValidBlockingHit())
		{
			TargetCameraLocation += HitResult.Location - HitResult.TraceEnd;
		}

		// Step 8: Lerp First Person Override and return target camera parameters.
		FTransform TargetCameraTransform(TargetCameraRotation, TargetCameraLocation);
		FTransform FPTargetCameraTransform(TargetCameraRotation, FPTarget);

		const FTransform& MixedTransform = UKismetMathLibrary::TLerp(TargetCameraTransform, FPTargetCameraTransform,
		                                                             GetCameraBehaviorParam(
			                                                             CurveName_WeightFirstPerson));

		const FTransform& TargetTransform = UKismetMathLibrary::TLerp(MixedTransform,
		                                                              FTransform(DebugViewRotation,
		                                                              	TargetCameraLocation),
		                                                              GetCameraBehaviorParam(CurveName_OverrideDebug));

		CameraInfo.Location = TargetTransform.GetLocation();
		CameraInfo.Rotation = TargetTransform.Rotator();
		CameraInfo.FOV = FMath::Lerp(TPFOV, FPFOV, GetCameraBehaviorParam(CurveName_WeightFirstPerson));
	}

	return CameraInfo;
}

FVector ABasePlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation,
                                                              FRotator CameraRotation, FVector LagSpeeds,
                                                              float DeltaTime) const
{
	CameraRotation.Roll = 0.0f;
	CameraRotation.Pitch = 0.0f;
	const FVector UnrotatedCurLoc = CameraRotation.UnrotateVector(CurrentLocation);
	const FVector UnrotatedTargetLoc = CameraRotation.UnrotateVector(TargetLocation);

	const FVector ResultVector(
		FMath::FInterpTo(UnrotatedCurLoc.X, UnrotatedTargetLoc.X, DeltaTime, LagSpeeds.X),
		FMath::FInterpTo(UnrotatedCurLoc.Y, UnrotatedTargetLoc.Y, DeltaTime, LagSpeeds.Y),
		FMath::FInterpTo(UnrotatedCurLoc.Z, UnrotatedTargetLoc.Z, DeltaTime, LagSpeeds.Z));

	return CameraRotation.RotateVector(ResultVector);
}

float ABasePlayerCameraManager::GetCameraBehaviorParam(FName CurveName) const
{
	if (auto AnimInstance = CameraBehavior->GetAnimInstance())
		return AnimInstance->GetCurveValue(CurveName);
	return 0.0f;
}
