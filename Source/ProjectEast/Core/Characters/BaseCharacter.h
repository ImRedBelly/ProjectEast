﻿#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Interfaces/CameraParameters.h"
#include "Interfaces/CharacterInfo.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Utils/Structurs.h"
#include "BaseCharacter.generated.h"

class UInputMappingContext;
//class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PROJECTEAST_API ABaseCharacter : public ACharacter, public ICameraParameters, public ICharacterInfo
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FDataTableRowHandle MovementModel;
	UPROPERTY(EditDefaultsOnly)
	EGait DesiredGait = EGait::Running;
	UPROPERTY(EditDefaultsOnly)
	ERotationMode DesiredRotationMode = ERotationMode::LookingDirection;
	UPROPERTY(EditDefaultsOnly)
	EViewMode DesiredEViewMode = EViewMode::ThirdPerson;
	UPROPERTY(EditDefaultsOnly)
	EOverlayState DesiredOverlayState = EOverlayState::Default;
	UPROPERTY(EditDefaultsOnly)
	EStance DesiredStance = EStance::Standing;

	UPROPERTY(EditDefaultsOnly, Category="Camera Settings")
	float LookUpDownRate = 1.25f;
	UPROPERTY(EditDefaultsOnly, Category="Camera Settings")
	float LookLeftRightRate = 1.25f;

#pragma region Camera
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	//UCameraComponent* CameraComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	//USpringArmComponent* SpringArmComponent;
#pragma endregion Camera

#pragma region InputActions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* WalkAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
#pragma endregion InputActions

#pragma region MultiTapInputCrouch
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool BreakFall;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool IsMultiTapInputCrouch;

	UFUNCTION(BlueprintImplementableEvent)
	void SetBreakFall();
	UFUNCTION(BlueprintImplementableEvent)
	void CheckMultiTapInputCrouch();
#pragma endregion MultiTapInputCrouch

private:
	UPROPERTY()
	UAnimInstance* MainAnimInstance;

	EGait AllowedGait = EGait::Walking;
	EGait ActualGait = EGait::Walking;

	EGait Gait = EGait::Walking;
	EGait PreviousActualGait = EGait::Walking;

	EMovementState MovementState = EMovementState::Grounded;
	EMovementState PreviousMovementState = EMovementState::Grounded;

	EMovementAction MovementAction = EMovementAction::None;
	EMovementAction PreviousMovementAction = EMovementAction::None;

	ERotationMode RotationMode = ERotationMode::LookingDirection;
	ERotationMode PreviousRotationMode = ERotationMode::LookingDirection;

	EViewMode ViewMode = EViewMode::ThirdPerson;
	EViewMode PreviousViewMode = EViewMode::ThirdPerson;

	EOverlayState OverlayState = EOverlayState::Default;
	EOverlayState PreviousOverlayState = EOverlayState::Default;

	EStance Stance = EStance::Standing;
	EStance PreviousStance = EStance::Standing;

	bool IsMoving;
	bool HasMovementInput;

	//Movement System
	FMovementSettingsState* MovementData;
	FMovementSettings CurrentMovementSettings;
	//Movement System

	FVector Acceleration;
	FVector PreviousVelocity;
	FVector LastPlayerMovementInput;
	FRotator LastVelocityRotation;
	FRotator LastGetPlayerMovementInput;
	FRotator LastMovementInputRotation;

	float Speed;
	float LocalWalkSpeed;
	float LocalRunSpeed;
	float LocalSprintSpeed;

	float MovementInputAmount;

	float AimYawRate;
	float PreviousAimYaw;


	ABaseCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void Jump() override;


#pragma region Input
	void OnBeginPlay();

	void OnMovementStateChanged(EMovementState NewMovementState);
	void OnMovementActionChanged(EMovementAction NewMovementAction);
	void OnRotationModeChanged(ERotationMode NewRotationMode);
	void OnGaitChanged(EGait NewGait);
	void OnViewModeChanged(EViewMode NewViewMode);
	void OnOverlayStateChanged(EOverlayState NewOverlayState);
	void OnStanceChanged(EStance NewStance);
#pragma endregion Initialize

#pragma region Input
	void PlayerMovementInput(const FInputActionValue& Value);
	void PlayerLookInput(const FInputActionValue& Value);
	void PlayerWalkInput(const FInputActionValue& Value);
	void PlayerStartSprintInput(const FInputActionValue& Value);
	void PlayerStopSprintInput(const FInputActionValue& Value);
	void PlayerCrouchInput(const FInputActionValue& Value);
	void PlayerStartAimInput(const FInputActionValue& Value);
	void PlayerStopAimInput(const FInputActionValue& Value);
#pragma endregion Input

#pragma region MovementSystem
	void OnCharacterMovementModeChanged(EMovementMode NewMovementMode);
	void SetMovementModel();
	void UpdateCharacterMovement();
	void UpdateDynamicMovementSettings(EGait InAllowedGait);
	FMovementSettings GetTargetMovementSettings() const;
	EGait GetAllowedGait() const;
	EGait GetActualGait(EGait InAllowedGait);
	bool CanSprint() const;
	float GetMappedSpeed() const;

	//void OnPlayBreakfallAnimation();
	//void OnPlayRollAnimation();
	//UAnimMontage* GetRollAnimation();

#pragma endregion MovementSystem

#pragma region SetNewStates
	void SetMovementState(EMovementState NewMovementState);
	void SetMovementAction(EMovementAction NewMovementAction);
	void SetRotationMode(ERotationMode NewRotationMode);
	void SetGait(EGait NewGait);
	void SetViewMode(EViewMode NewViewMode);
	void SetOverlayState(EOverlayState NewOverlayState);
#pragma endregion SetNewStates

#pragma region RotationSystem
private:
	FRotator TargetRotation;
	FRotator InAirRotation;
	float YawOffset;

	void UpdateGroundedRotation();
	void UpdateInAirRotation();
	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed);
	void AddToCharacterRotation(FRotator DeltaRotation);
	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed);
	TTuple<FHitResult*, bool> CustomSetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bSweep, bool bTeleport);
	float CalculateGroundedRotationRate() const;
	bool CanUpdateMovingRotation() const;
#pragma endregion RotationSystem

#pragma region MantleSystem
	FMantleParams MantleParams;
	FComponentAndTransform MantleLedgeLS;
	FTransform MantleTarget;
	FTransform MantleActualStartOffset;
	FTransform MantleAnimatedStartOffset;

protected:
	UPROPERTY(EditAnywhere)
	FMantleTraceSettings GroundedTraceSettings;
	UPROPERTY(EditAnywhere)
	FMantleTraceSettings AutomaticTraceSettings;
	UPROPERTY(EditAnywhere)
	FMantleTraceSettings FallingTraceSettings;

	bool MantleCheck(FMantleTraceSettings TraceSettings, EDrawDebugTrace::Type DebugTrace);
	//void MantleStart(float MantleHeight, FComponentAndTransform MantleLedgeWS, EMantleType MantleType);
	//void MantleEnd();
	//void MantleUpdate(float BlendIn);
	//bool CapsuleHasRoomCheck(UCapsuleComponent* Capsule, FVector TargetLocation, float HeightOffset, float RadiusOffset, EDrawDebugTrace::Type DebugType);
	//FMantleAsset GetMantleAsset(EMantleType MantleType);
#pragma endregion MantleSystem

#pragma region RagdollSystem
	bool RagdollOnGround;
	bool RagdollFaceUp;
	FVector LastRagdollVelocity;

	//bool RagdollStart();
	//bool RagdollEnd();
	//bool RagdollUpdate();
	//bool SetActorLocationDuringRagdoll();
	//UAnimMontage* GetGetUpAnimation(bool RagdollFaceUp);

#pragma endregion RagdollSystem

#pragma region ICameraParameters

public:
	virtual FCameraViewParameters GetCameraParameters() override;
	virtual FVector GetFPCameraTarget() override;
	virtual FTransform GetTPPivotTarget() override;
	virtual FTPTraceParams GetTPTraceParams() override;

protected:
	UPROPERTY(EditAnywhere, Category="Camera System")
	float ThirdPersonFOV = 80;
	UPROPERTY(EditAnywhere, Category="Camera System")
	float FirstPersonFOV = 80;
	UPROPERTY(EditAnywhere, Category="Camera System")
	bool RightShoulder = true;

#pragma endregion ICameraParameters


#pragma region ICharacterInfo

public:
	virtual FCharacterStates GetCurrentStates() override;
	virtual FEssentialValues GetEssentialValues() override;

private:
	FCharacterStates CharacterStates;
	FEssentialValues EssentialValues;

#pragma endregion ICharacterInfo

#pragma region EssentialInformation
	void SetEssentialValues(float DeltaTime);
	void CacheValues();
#pragma endregion EssentialInformation

#pragma region Utility
	TTuple<FVector, FVector> GetControlForwardRightVector() const;
	FVector GetCapsuleBaseLocation(float ZOffset) const;
	//FVector GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset) const;
	float GetAnimCurveValue(FName CurveName) const;
	TTuple<float, float> FixDiagonalGamepadValues(float Y, float X);
#pragma endregion Utility

#pragma region Debug
	void DrawDebugShapes();
	EDrawDebugTrace::Type GetTraceDebugType(EDrawDebugTrace::Type DebugTrace);
#pragma endregion Debug
};