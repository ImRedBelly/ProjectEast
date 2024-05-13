#include "PlayerAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"
#include "ProjectEast/Core/Characters/PlayerCharacter.h"
#include "ProjectEast/Core/Components/Movement/PlayerMovementComponent.h"

class APlayerCharacter;

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// checkf(!TryGetPawnOwner()->IsA<APlayerCharacter>(),
	// 	   TEXT(
	// 		   "UGCBasePawnAnimInstance::NativeBeginPlay only game code GameCodeBasePawn can work with UGCBasePawnAnimInstance"
	// 	   ));
	CachedPlayerCharacter = StaticCast<APlayerCharacter*>(TryGetPawnOwner());
}


void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!CachedPlayerCharacter.IsValid())
		return;

	UPlayerMovementComponent* CharacterMovement = CachedPlayerCharacter->GetPlayerMovementComponent();

	DirectionMove = CharacterMovement->Velocity;
	Speed = CharacterMovement->Velocity.Size();

	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsSliding = CharacterMovement->IsSliding();
	AimRotation = CachedPlayerCharacter->GetAimOffset();
}
