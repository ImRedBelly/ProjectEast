#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"


class APlayerCharacter;

UCLASS()
class PROJECTEAST_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character animation", meta=(UIMin = 0, UIMax = 500))
	FVector DirectionMove;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation", meta=(UIMin = 0, UIMax = 500))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character animation")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character animation")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character animation")
	bool bIsSliding = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character animation")
	FRotator AimRotation = FRotator::ZeroRotator;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	TWeakObjectPtr<APlayerCharacter> CachedPlayerCharacter;

};
