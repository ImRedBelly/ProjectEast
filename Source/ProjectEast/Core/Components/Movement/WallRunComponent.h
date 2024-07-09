#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunComponent.generated.h"

UENUM(BlueprintType)
enum class EDirectionType : uint8
{
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
};

UENUM(BlueprintType)
enum class EWallRunType : uint8
{
	Default UMETA(DisplayName = "Default"),
	HighSpeedInfiniteDurationRequiresTagNoArc UMETA(DisplayName = "HighSpeedInfiniteDurationRequiresTagNoArc"),
	DegreeCornersSharpWallJumpAngle UMETA(DisplayName = "DegreeCornersSharpWallJumpAngle"),
	HigherArcHigherWallJump UMETA(DisplayName = "HigherArcHigherWallJump"),
};

USTRUCT(BlueprintType)
struct FWallRunParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	EWallRunType WallRunType = EWallRunType::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float WallRunSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float ArcAmount = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float WallRunDurationLimit = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float InsideCornerAngleLimit = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float OutsideCornerAngleLimit = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float WallJumpAngle = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float WallJumpHorizontalVelocity = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float WallJumpVerticalVelocity = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	float WallJumpAirControl = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Core Utilities | Wall Run Movement Settings")
	float WallRunAcceleration = 50000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Core Utilities | Wall Run Movement Settings")
	float WallRunBrakingDeceleration = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	bool RequiresTag;

};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UWallRunComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWallRunComponent();
	bool StartWallJump();
	void StopWallJump();
	bool StartWallRun(EDirectionType Direction);
	void StopWallRun(EMovementMode NewMovementMode, float TemporarilyDisableWallRunSeconds);
	void DetectWall(EDirectionType DirectionType);

	bool GetIsWallRunning() const { return bIsWallRunning; }
	bool GetIsWallJumping() const { return bIsWallJumping; }
	EDirectionType GetDirectionType() const { return WallSide; }
	float GetCurrentArcAngle() const { return CurrentArcAngle; }
	float GetCurrentTurnRate() const { return CurrentTurnRate; }
	float GetWallJumpHorizontalVelocity() const { return CurrentWallRunParameters.WallJumpHorizontalVelocity; }
	float GetWallJumpVerticalVelocity() const { return CurrentWallRunParameters.WallJumpVerticalVelocity; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	EWallRunType CurrentWallRunType = EWallRunType::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	TArray<FWallRunParameters> WallRunParameters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	FName TagForPreventingWallRun = "NoWallRun";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wall Run Parameters")
	FName TagForAllowingWallRun = "WallRun";


	FWallRunParameters CurrentWallRunParameters;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	TWeakObjectPtr<ACharacter> CachedCharacterOwner;

	EDirectionType WallSide;

	FVector CurrentWallLocation;
	FVector PrimaryTraceNormal;
	FVector SecondaryTraceNormal;
	FVector AverageWallNormal;
	FVector PreviousVelocity;


	FRotator PreviousRotation;


	float CapsuleRadius;
	float DefaultMaxFlySpeed;
	float DefaultMaxAcceleration;
	float DefaultBrakingDeceleration;
	float DefaultAirControl;
	float CurrentCornerAngle;
	float LastKnownSafeHeight;
	float CurrentWallRunDuration;
	float DistanceFromWall;
	float PrimaryTraceSize;
	float CurrentArcAngle;
	float CurrentTurnRate;
	float DistanceAboveWall;

	bool bIsWallRunning;
	bool bIsWallJumping;
	bool bSpeedTooLow;
	bool DefaultOrientRotation = false;
	bool WallRunningAvailable = true;

	FTimerHandle DurationCountdownTimer;
	FTimerHandle DisabledWallRunTimer;
	FTimerHandle SpeedMonitorTimer;
	FTimerHandle WallJumpAirControlTimer;

	void WallRunTick(float DeltaSeconds);
	void WallJumpTick(float DeltaSeconds) const;
	void GatherCharacterInformation();
	bool IsWallRunnable(const AActor* WallActor) const;
	FHitResult LineTraceRelativeToCapsuleAndWall(FVector CurrentWallNormal, FVector CurrentWallLocation,
	                                             float InitialTraceLength, float StartOffset_Backwards,
	                                             float StartOffset_AwayFromWall, float EndOffset_Backwards,
	                                             float EndOffset_AwayFromWall, EDirectionType DirectionType,
	                                             bool ShowDebugLines) const;

	bool CheckCornerAngle(FVector FirstWallNormal, FVector SecondWallNormal);
	void CheckEnoughVerticalSpace(EDirectionType DirectionType);
	bool BottomHeightCheck(EDirectionType DirectionType);
	bool TopHeightCheck();
	bool SetLastKnownSafeHeight();
	bool SetLastRecordedSafeHeight() const;
	bool SetDesiredMaxHeightForCurrentWall();
	void HugWall() const;

	bool BackupCapsuleTrace(FVector StartLocation);
	void UpdateAnimationsValue(float DeltaSeconds);
	void DelayedAirControl();
	void EnableWallJumpAirControl() const;
	void StopMonitoringSpeed();
	void TemporarilyDisableWallRunning(float DisableTime);
	void UpdateRotationALS();
	void StartDurationCountdown();
	void StartMonitoringSpeed();
	void AllowWallRunning();
	void MonitorSpeed();
	void WallRunTimeout();

	UFUNCTION()
	void OnCharacterMovement(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	UFUNCTION()
	void OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	void EnterGate();
	void OpenGate();
	void CloseGate();
	bool bGateOpen;
};
