#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UPlayerMovementComponent;
struct FInputActionValue;

UCLASS()
class PROJECTEAST_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE UPlayerMovementComponent* GetPlayerMovementComponent() const
	{
		return PlayerMovementComponent;
	}

	FRotator GetAimOffset() const;
	void SetStateStrafe(bool state) const;

protected:
#pragma region InputActions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SlidingAction;

#pragma endregion InputActions

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	USpringArmComponent* SpringArmComponent;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

#pragma region Jump

	virtual void Jump() override;

#pragma endregion Jump

#pragma region Sprint

	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void TryChangeSprintState();

#pragma endregion Sprint

#pragma region CameraArm

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float NormalCameraTargetArmLength = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float SprintCameraTargetArmLength = 400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float CrouchCameraTargetArmLength = 150;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float AimCameraTargetArmLength = 100;

#pragma endregion CameraArm


#pragma region Crouch

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settighs | Crouch")
	float HeightCheckUncrouch;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void ChangeCrouchState();
	bool CheckHeightForUncrouch() const;

#pragma endregion Crouch

private:
	UPROPERTY()
	UPlayerMovementComponent* PlayerMovementComponent;

#pragma region Sprint

	bool bIsSprintRequested = false;

#pragma endregion Sprint

#pragma region Crouch

	bool bIsCrouchRequested = false;

#pragma endregion Crouch

#pragma region CameraArm

	bool bIsChangeTargetArmLength = false;
	float CurrentTargetArmLength = 300;

	void TryChangeTargetArmLength();
	void SetTargetArmLength();
	void SetTargetOffset(bool IsAppend, float NewTargetOffsetZ) const;
	float GetTargetArm() const;
#pragma endregion CameraArm

#pragma region Aiming

	bool bIsAiming = false;

	void OnStartAiming(const FInputActionValue& Value);
	void OnEndAiming(const FInputActionValue& Value);
	void SetStateAiming(bool IsAiming);

#pragma endregion Aiming


#pragma region Sliding

	bool bIsSliding = false;
	float TargetOffsetHalfHeightInSliding = 28;

	void OnStartSliding();
	void OnEndSliding();
	
protected:
	FTimerHandle SlidingTimer;
	
#pragma endregion Aiming
};
