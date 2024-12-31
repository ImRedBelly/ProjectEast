#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerWallRunAnimInstance.generated.h"

class ABaseCharacter;
enum class EDirectionType : uint8;

UCLASS()
class PROJECTEAST_API UPlayerWallRunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
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


	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	TWeakObjectPtr<ABaseCharacter> CachedPlayerCharacter;
};
