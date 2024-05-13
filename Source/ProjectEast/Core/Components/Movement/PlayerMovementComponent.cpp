#include "PlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

float UPlayerMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting && !bIsSliding)
		Result = SprintSpeed;
	else if (bIsSliding)
		Result = SlidingSpeed;

	return Result;
}

// void UPlayerMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
// {
// 	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
//
// 	auto a = FString::FromInt(PreviousCustomMode);
// 	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, a);
//
// 	if (MovementMode == MOVE_Walking && (ECustomMovementMode)PreviousCustomMode == ECustomMovementMode::CMOVE_Sliding)
// 	{
// 	}
//
// 	else if (PreviousMovementMode == MOVE_Walking && (ECustomMovementMode)PreviousCustomMode ==
// 		ECustomMovementMode::CMOVE_Sliding)
// 	{
// 	}
// }

void UPlayerMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UPlayerMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

void UPlayerMovementComponent::StartSliding()
{
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
	bIsSliding = true;
	bForceMaxAccel = 1;
}

void UPlayerMovementComponent::StopSliding()
{
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(
		DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(),
		DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	
	bIsSliding = false;
	bForceMaxAccel = 0;
}

