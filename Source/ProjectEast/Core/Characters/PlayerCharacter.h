﻿#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UEquipmentMeshUpdater;
class AInteractiveActor;
enum class EDirectionType : uint8;
class UWallRunComponent;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
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

	FORCEINLINE UWallRunComponent* GetWallRunComponent() const
	{
		return WallRunComponent;
	}

	FRotator GetAimOffset() const;
	void SetStateStrafe(bool state) const;

protected:
#pragma region InputActions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* GeneralMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* UIMappingContext;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UsePocket1Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UsePocket2Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UsePocket3Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UsePocket4Action;

	UInputMappingContext* CurrentMappingContext;

#pragma endregion InputActions

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	USpringArmComponent* SpringArmComponent;

public:
	void SetGeneralMappingContext();
	void SetUIMappingContext();
	void SetMappingContext(UInputMappingContext* MappingContext);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

#pragma region Jump

	virtual void Jump() override;

#pragma endregion Jump

#pragma region Sprint

public:
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void TryChangeSprintState();

private:
	bool bIsSprintRequested = false;

#pragma endregion Sprint

#pragma region CameraArm

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float NormalCameraTargetArmLength = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float SprintCameraTargetArmLength = 400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float CrouchCameraTargetArmLength = 150;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	float AimCameraTargetArmLength = 100;

private:
	bool bIsChangeTargetArmLength = false;
	float CurrentTargetArmLength = 300;

	void TryChangeTargetArmLength();
	void SetTargetArmLength();
	void SetTargetOffset(bool IsAppend, float NewTargetOffsetZ) const;
	float GetTargetArm() const;

#pragma endregion CameraArm

#pragma region Crouch

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settighs | Crouch")
	float HeightCheckUncrouch;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void ChangeCrouchState();
	bool CheckHeightForUncrouch() const;

private:
	bool bIsCrouchRequested = false;

#pragma endregion Crouch

private:
	UPROPERTY()
	UPlayerMovementComponent* PlayerMovementComponent;

#pragma region Aiming

public:
	bool IsStrafing() const { return bIsAiming; }

private:
	bool bIsAiming = false;

	void OnStartAiming(const FInputActionValue& Value);
	void OnEndAiming(const FInputActionValue& Value);
	void SetStateAiming(bool IsAiming);

#pragma endregion Aiming

#pragma region Ledge

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character|Movement")
	UWallRunComponent* WallRunComponent;

#pragma endregion Ledge

#pragma region EquipmentMesh

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character|Equipment")
	UEquipmentMeshUpdater* EquipmentMeshUpdater;

	UPROPERTY(VisibleAnywhere, Category="Character|Equipment")
	USkeletalMeshComponent* SkeletalMeshHead;
	UPROPERTY(VisibleAnywhere, Category="Character|Equipment")
	USkeletalMeshComponent* SkeletalMeshGloves;
	UPROPERTY(VisibleAnywhere, Category="Character|Equipment")
	USkeletalMeshComponent* SkeletalMeshChest;
	UPROPERTY(VisibleAnywhere, Category="Character|Equipment")
	USkeletalMeshComponent* SkeletalMeshLegs;
	UPROPERTY(VisibleAnywhere, Category="Character|Equipment")
	USkeletalMeshComponent* SkeletalMeshBoots;

#pragma endregion Ledge

#pragma region Sliding

	bool bIsSliding = false;
	float TargetOffsetHalfHeightInSliding = 30;

	void OnStartSliding();
	void OnEndSliding();

protected:
	FTimerHandle SlidingTimer;

#pragma endregion Sliding

#pragma region Interactive

private:
	void OnInteractive();
	void OnOpenInventory();
	void OnPause();
	void UsePocket1();
	void UsePocket2();
	void UsePocket3();
	void UsePocket4();

#pragma endregion Interactive

#pragma region IKSetting

public:
	float GetIKRightFootOffset() const { return IKRightFootOffset; }
	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	float GetIKPelvisOffset() const { return IKPelvisOffset; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKTraceDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKInterpSpeed = 30.f;

private:
	float IKLeftFootOffset = 0.0f;
	float IKRightFootOffset = 0.0f;
	float IKPelvisOffset = 0.0f;

	void UpdateIKSettings(float DeltaSeconds);
	float CalculateIKPelvisOffset() const;
	float CalculateIKParametersForSocketName(const FName& SocketName) const;
#pragma endregion IKSetting
};
