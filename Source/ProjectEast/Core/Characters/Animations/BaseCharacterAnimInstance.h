#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectEast/Core/Library/ALSAnimationStructLibrary.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Utils/Structurs.h"
#include "BaseCharacterAnimInstance.generated.h"

class ABaseCharacter;
class ICharacterInfo;
class ICameraParameters;

UCLASS()
class PROJECTEAST_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	/** Configuration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Main Configuration", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimConfiguration Config;

	/** Anim Graph - Layer Blending */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimGraphLayerBlending LayerBlendingValues;

	/** Anim Graph - Foot IK */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Foot IK", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimGraphFootIK FootIKValues;

	/** Turn In Place */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Turn In Place", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimTurnInPlace TurnInPlaceValues;

	/** Rotate In Place */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Rotate In Place", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimRotateInPlace RotateInPlace;


#pragma region References
	UPROPERTY(BlueprintReadOnly, Category = "Read Only Data|Character Information")
	TObjectPtr<ABaseCharacter> Character = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimCharacterInformation CharacterInformation;

	TWeakInterfacePtr<ICharacterInfo> CharacterInfo;
	TWeakInterfacePtr<ICameraParameters> CameraParameters;

	float DeltaTimeX;

#pragma endregion References

#pragma region CharacterInformation

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EMovementState MovementState = EMovementState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EMovementState PrevMovementState = EMovementState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EMovementAction MovementAction = EMovementAction::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	ERotationMode RotationMode = ERotationMode::VelocityDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EViewMode ViewMode = EViewMode::ThirdPerson;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EGait Gait = EGait::Walking;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EStance Stance = EStance::Standing;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EOverlayState OverlayState = EOverlayState::Default;

#pragma endregion CharacterInformation

#pragma region Anim Graph - Grounded

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FALSAnimGraphGrounded Grounded;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FALSVelocityBlend VelocityBlend;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FALSLeanAmount LeanAmount;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FVector RelativeAccelerationAmount = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	EGroundedEntryState GroundedEntryState = EGroundedEntryState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	EMovementDirection MovementDirection = EMovementDirection::Forward;

	ETrackedHipsDirection TrackedHipsDirection;


#pragma endregion Anim Graph - Grounded

#pragma region Anim Graph - In Air

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - In Air", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimGraphInAir InAir;

#pragma endregion Anim Graph - In Air

#pragma region Anim Graph - Aiming Values

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Aiming Values", Meta = (
		ShowOnlyInnerProperties))
	FALSAnimGraphAimingValues AimingValues;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Aiming Values")
	FVector2D SmoothedAimingAngle = FVector2D::ZeroVector;

#pragma endregion Anim Graph - Aiming Values

#pragma region Anim Graph - Ragdoll

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Ragdoll")
	float FlailRate = 0.0f;

#pragma endregion Anim Graph - Ragdoll

#pragma region Blend Curves

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> DiagonalScaleAmountCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> StrideBlend_N_Walk = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> StrideBlend_N_Run = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> StrideBlend_C_Walk = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> LandPredictionCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> LeanInAirCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveVector> YawOffset_FB = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveVector> YawOffset_LR = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Dynamic Transition")
	TObjectPtr<UAnimSequenceBase> TransitionAnim_R = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Dynamic Transition")
	TObjectPtr<UAnimSequenceBase> TransitionAnim_L = nullptr;

#pragma endregion Blend Curves

#pragma region IK Bone Names

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Anim Graph - Foot IK")
	FName IkFootL_BoneName = FName(TEXT("ik_foot_l"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Anim Graph - Foot IK")
	FName IkFootR_BoneName = FName(TEXT("ik_foot_r"));

#pragma endregion IK Bone Names


	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation")
	void PlayTransition(const FALSDynamicMontageParams& Parameters);

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation")
	void PlayTransitionChecked(const FALSDynamicMontageParams& Parameters);

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation")
	void PlayDynamicTransition(float ReTriggerDelay, FALSDynamicMontageParams Parameters);

	UFUNCTION(BlueprintCallable, Category = "ALS|Event")
	void OnJumped();

	UFUNCTION(BlueprintCallable, Category = "ALS|Event")
	void OnPivot();


	UFUNCTION(BlueprintCallable, Category = "ALS|Grounded")
	void SetTrackedHipsDirection(ETrackedHipsDirection HipsDirection)
	{
		Grounded.TrackedHipsDirection = HipsDirection;
	}

	UFUNCTION(BlueprintCallable, Category = "ALS|Grounded")
	void SetGroundedEntryState(EGroundedEntryState NewState)
	{
		GroundedEntryState = NewState;
	}

#pragma region Update Values

	void UpdateCharacterInfo();
	void UpdateAimingValues(float DeltaSeconds);
	void UpdateLayerValues();
	void UpdateFootIK(float DeltaSeconds);
	void UpdateMovementValues();
	void UpdateRotationValues();
	void UpdateInAirValues();
	void UpdateRagdollValues();

#pragma endregion Update Values

#pragma region Grounded

	bool ShouldMoveCheck() const;
	bool CanTurnInPlace() const;
	bool CanRotateInPlace() const;
	bool CanDynamicTransition() const;
	bool CanOverlayTransition() const;
	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurrent);
	void TurnInPlaceCheck();
	void RotateInPlaceCheck();
	void DynamicTransitionCheck();

#pragma endregion Grounded

#pragma region Movement

	FALSVelocityBlend CalculateVelocityBlend() const;
	float CalculateDiagonalScaleAmount() const;
	FVector CalculateRelativeAccelerationAmount() const;
	float CalculateWalkRunBlend() const;
	float CalculateStrideBlend() const;
	float CalculateStandingPlayRate() const;
	float CalculateCrouchingPlayRate() const;

#pragma endregion Movement

#pragma region In Air

	float CalculateLandPrediction() const;
	FALSLeanAmount CalculateAirLeanAmount() const;

#pragma endregion In Air

#pragma region Foot IK

	void SetFootOffsets(float DeltaSeconds, FName EnableFootIKCurve, FName IKFootBone,
	                    FName RootBone, FVector& CurLocationTarget, FVector& CurLocationOffset,
	                    FRotator& CurRotationOffset);
	void SetPelvisIKOffset(float DeltaSeconds, FVector FootOffsetLTarget, FVector FootOffsetRTarget);
	void SetFootLocking(float DeltaSeconds, FName EnableFootIKCurve, FName FootLockCurve,
	                    FName IKFootBone, float& CurFootLockAlpha, bool& UseFootLockCurve,
	                    FVector& CurFootLockLoc, FRotator& CurFootLockRot);
	void SetFootLockOffsets(float DeltaSeconds, FVector& LocalLocation, FRotator& LocalRotation) const;
	void ResetIKOffsets(float DeltaSeconds);

#pragma endregion Foot IK

#pragma region Rotation

	EMovementDirection CalculateMovementDirection() const;

#pragma endregion Rotation

#pragma region Interpolation
	FALSVelocityBlend InterpVelocityBlend(FALSVelocityBlend Current, FALSVelocityBlend Target, float InterpSpeed,
	                                      float DeltaTime);
	FALSLeanAmount InterpLeanAmount(FALSLeanAmount Current, FALSLeanAmount Target, float InterpSpeed, float DeltaTime);
#pragma endregion Interpolation

	float GetAnimCurveCompact(FName CurveName) const;
	float GetAnimCurveClamped(FName CurveName, float Bias, float ClampMin, float ClampMax) const;

private:
	FTimerHandle PlayDynamicTransitionTimer;
	FTimerHandle OnJumpedTimer;
	FTimerHandle OnPivotTimer;
	
	bool bCanPlayDynamicTransition = true;
	
	void PlayDynamicTransitionDelay();
	void OnJumpedDelay();
	void OnPivotDelay();
};
