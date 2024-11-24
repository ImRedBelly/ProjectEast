#include "PlayerCameraAnimInstance.h"

#include "ProjectEast/Core/Characters/BaseCharacter.h"
#include "ProjectEast/Core/Characters/Interfaces/CharacterInfo.h"

void UPlayerCameraAnimInstance::InitializeAnimInstance(APawn* InControlledPawn)
{
	ControlledPawn = InControlledPawn;
	if (InControlledPawn->GetClass()->ImplementsInterface(UCameraParameters::StaticClass()))
		CameraParameters = Cast<ICameraParameters>(ControlledPawn);
	if (InControlledPawn->GetClass()->ImplementsInterface(UCharacterInfo::StaticClass()))
		CharacterInfo = Cast<ICharacterInfo>(ControlledPawn);
}

void UPlayerCameraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateCharacterInfo();
}

void UPlayerCameraAnimInstance::UpdateCharacterInfo()
{
	if (CharacterInfo.IsValid())
	{
		MovementState = CharacterInfo->GetCurrentStates().MovementState;
		MovementAction = CharacterInfo->GetCurrentStates().MovementAction;
		RotationMode = CharacterInfo->GetCurrentStates().RotationMode;
		Gait = CharacterInfo->GetCurrentStates().ActualGait;
		Stance = CharacterInfo->GetCurrentStates().ActualStance;
		ViewMode = CharacterInfo->GetCurrentStates().ViewMode;
	}

	if (CameraParameters.IsValid())
		RightShoulder = CameraParameters->GetCameraParameters().RightShoulder;
}
