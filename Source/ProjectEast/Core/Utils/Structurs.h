#pragma once

#include "GameTypes.h"
#include "Curves/CurveVector.h"
#include "Structurs.generated.h"

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float RunSpeed;
	UPROPERTY(EditAnywhere)
	float SprintSpeed;
	UPROPERTY(EditAnywhere)
	UCurveVector* MovementCurve;
	UPROPERTY(EditAnywhere)
	UCurveFloat* RotationRateCurve;
};

USTRUCT(BlueprintType)
struct FMovementSettingsStance
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	FMovementSettings Standing;
	UPROPERTY(EditDefaultsOnly)
	FMovementSettings Crouching;
};

USTRUCT(BlueprintType)
struct FMovementSettingsState : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	FMovementSettingsStance VelocityDirection;
	UPROPERTY(EditDefaultsOnly)
	FMovementSettingsStance LookingDirection;
	UPROPERTY(EditDefaultsOnly)
	FMovementSettingsStance Aiming;
};

USTRUCT(BlueprintType)
struct FMantleTraceSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	float MaxLedgeHeight;
	UPROPERTY(EditDefaultsOnly)
	float MinLedgeHeight;
	UPROPERTY(EditDefaultsOnly)
	float ReachDistance;
	UPROPERTY(EditDefaultsOnly)
	float ForwardTraceRadius;
	UPROPERTY(EditDefaultsOnly)
	float DownwardTraceRadius;
};

USTRUCT(BlueprintType)
struct FMantleParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimMontage;
	UPROPERTY(EditDefaultsOnly)
	UCurveVector* PositionCorrectionCurve;
	UPROPERTY(EditDefaultsOnly)
	FVector StartingOffset;
	UPROPERTY(EditDefaultsOnly)
	float StartingPosition;
	UPROPERTY(EditDefaultsOnly)
	float PlayRate;
};

USTRUCT(BlueprintType)
struct FComponentAndTransform
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	FTransform Transform;
	UPROPERTY(EditDefaultsOnly)
	UPrimitiveComponent* Component;
};

USTRUCT(BlueprintType)
struct FMantleAsset
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimMontage;
	UPROPERTY(EditDefaultsOnly)
	UCurveVector* PositionCorrectionCurve;
	UPROPERTY(EditDefaultsOnly)
	FVector StartingOffset;
	UPROPERTY(EditDefaultsOnly)
	float LowHeight;
	UPROPERTY(EditDefaultsOnly)
	float LowPlayRate;
	UPROPERTY(EditDefaultsOnly)
	float LowStartPosition;
	UPROPERTY(EditDefaultsOnly)
	float HighHeight;
	UPROPERTY(EditDefaultsOnly)
	float HighPlayRate;
	UPROPERTY(EditDefaultsOnly)
	float HighStartPosition;
};

USTRUCT(BlueprintType)
struct FCameraInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector Location;
	UPROPERTY(BlueprintReadOnly)
	FRotator Rotation;
	UPROPERTY(BlueprintReadOnly)
	float FOV;
};

struct FCameraViewParameters
{
	float TPFOV;
	float FPFOV;
	bool RightShoulder;
};

struct FTPTraceParams
{
	FVector TraceOrigin;
	float TraceRadius;
	TEnumAsByte<ECollisionChannel> TraceChannel;
};

struct FCharacterStates
{
	EMovementMode PawnMovementMode;
	EMovementState MovementState;
	EMovementState PrevMovementState;

	EMovementAction MovementAction;
	ERotationMode RotationMode;
	EGait ActualGait;
	EStance ActualStance;
	EViewMode ViewMode;
	EOverlayState OverlayState;
};

struct FEssentialValues
{
	FVector Velocity;
	FVector Acceleration;
	FVector MovementInput;
	bool IsMoving;
	bool HasMovementInput;
	float Speed;
	float MovementInputAmount;
	FRotator AimingRotation;
	float AimYawRate;
};
