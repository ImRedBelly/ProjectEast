#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "PlayerCameraAnimInstance.generated.h"


class ICameraParameters;
class ICharacterInfo;
class ABaseCharacter;

UCLASS()
class PROJECTEAST_API UPlayerCameraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void InitializeAnimInstance(APawn* InControlledPawn);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	EMovementState MovementState = EMovementState::Grounded;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	EMovementAction MovementAction = EMovementAction::None;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	ERotationMode RotationMode = ERotationMode::VelocityDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	EGait Gait = EGait::Walking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	EStance Stance = EStance::Standing;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	EViewMode ViewMode = EViewMode::ThirdPerson;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	bool RightShoulder;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Info")
	bool DebugView = false;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	UPROPERTY()
	APlayerController* PlayerController;
	
	UPROPERTY()
	APawn* ControlledPawn;
	
	TWeakInterfacePtr<ICameraParameters> CameraParameters;
	TWeakInterfacePtr<ICharacterInfo> CharacterInfo;

	void UpdateCharacterInfo();
};
