#include "PlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "ProjectEast/Core/Characters/PlayerCharacter.h"

float UPlayerMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting && !bIsSliding)
		Result = SprintSpeed;
	else if (bIsSliding)
		Result = SlidingSpeed;

	return Result;
}

// APlayerCharacter* UPlayerMovementComponent::GetBaseCharacterOwner() const
// {
// 	return StaticCast<APlayerCharacter*>(CharacterOwner);
// }
//
// void UPlayerMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
// {
// 	switch (CustomMovementMode)
// 	{
// 	default: ;
// 	}
// 	Super::PhysCustom(deltaTime, Iterations);
// }
//
// void UPlayerMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
// {
// 	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
//
// 	if (MovementMode == MOVE_Custom)
// 	{
// 		switch (CustomMovementMode)
// 		{
// 		default:
// 			break;
// 		}
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
	CharacterOwner->GetMesh()->SetRelativeLocation(FVector(0,0,-58));
	bIsSliding = true;
	bForceMaxAccel = 1;
}

void UPlayerMovementComponent::StopSliding()
{
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(
		DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(),
		DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	CharacterOwner->GetMesh()->SetRelativeLocation(FVector(0,0,-88));

	bIsSliding = false;
	bForceMaxAccel = 0;
}
