#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ProjectEast/Core/Utils/Structurs.h"
#include "BasePlayerCameraManager.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PROJECTEAST_API ABasePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	void OnPossess(APawn* NewPawn);

protected:
	ABasePlayerCameraManager();

	UFUNCTION(BlueprintCallable)
	FCameraInfo CustomCameraBehavior();

private:
	UPROPERTY()
	APawn* ControlledPawn;
	UPROPERTY()
	USkeletalMeshComponent* CameraBehavior;

	FCameraInfo CameraInfo;

	FTransform SmoothedPivotTarget;
	FVector DebugViewOffset = FVector(350, 0, 50);
	FRotator DebugViewRotation = FRotator(-5, 180, 0);

	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation,
	                                    FVector LagSpeeds, float DeltaTime) const;
	float GetCameraBehaviorParam(FName CurveName) const;

	const FName CurveName_WeightFirstPerson = FName("Weight_FirstPerson");
	const FName CurveName_RotationLagSpeed = FName("RotationLagSpeed");
	const FName CurveName_OverrideDebug = FName("Override_Debug");
	const FName CurveName_PivotLagSpeedX = FName("PivotLagSpeed_X");
	const FName CurveName_PivotLagSpeedY = FName("PivotLagSpeed_Y");
	const FName CurveName_PivotLagSpeedZ = FName("PivotLagSpeed_Z");

	const FName CurveName_PivotOffsetX = FName("PivotOffset_X");
	const FName CurveName_PivotOffsetY = FName("PivotOffset_Y");
	const FName CurveName_PivotOffsetZ = FName("PivotOffset_Z");

	const FName CurveName_CameraOffsetX = FName("CameraOffset_X");
	const FName CurveName_CameraOffsetY = FName("CameraOffset_Y");
	const FName CurveName_CameraOffsetZ = FName("CameraOffset_Z");
};
