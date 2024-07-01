#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"


enum class EDirectionType : uint8;
class APlayerCharacter;

UCLASS()
class PROJECTEAST_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation", meta=(UIMin = 0, UIMax = 500))
	FVector DirectionMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation",meta = (UIMin = -180.0f, UIMax = 180.0f));
	float Direction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation", meta=(UIMin = 0, UIMax = 500))
	float Speed;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Character animation")
	bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation")
	bool bIsSliding = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsStrafing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation")
	FRotator AimRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	bool bIsWallRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	bool bIsWallJumping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	float CurrentArcAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	float CurrentTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	float WallJumpHorizontalVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	float WallJumpVerticalVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	float CurrentAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	float WallJumpAnimationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	float HandCorrectionAdditive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation | Wall Run")
	EDirectionType DirectionType;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
	FVector IKRightFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
	FVector IKLeftFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
	FVector IKPelvisBoneOffset = FVector::ZeroVector;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	TWeakObjectPtr<APlayerCharacter> CachedPlayerCharacter;
};
