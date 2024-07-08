#include "PlayerAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ProjectEast/Core/Characters/PlayerCharacter.h"
#include "ProjectEast/Core/Components/Movement/WallRunComponent.h"
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
	UWallRunComponent* WallRunComponent = CachedPlayerCharacter->GetWallRunComponent();

	DirectionMove = CharacterMovement->Velocity;

	Speed = CharacterMovement->Velocity.Size();
	Direction = FMathf::Round(UKismetAnimationLibrary::CalculateDirection(CharacterMovement->Velocity, CachedPlayerCharacter->GetActorRotation()));
	
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsSliding = CharacterMovement->IsSliding();
	bIsStrafing = CachedPlayerCharacter->IsStrafing();
	AimRotation = CachedPlayerCharacter->GetAimOffset();
	
	bIsWallRunning = WallRunComponent->GetIsWallRunning();
	bIsWallJumping = WallRunComponent->GetIsWallJumping();
	DirectionType = WallRunComponent->GetDirectionType();
	CurrentArcAngle = WallRunComponent->GetCurrentArcAngle();
	CurrentTurnRate = WallRunComponent->GetCurrentTurnRate();
	CurrentAnimPlayRate = UKismetMathLibrary::MapRangeClamped(CharacterMovement->Velocity.Length(), 680.0f, 920.0f, 0.85f, 1.15f);

	float SizeSpeed = FVector(0.0f, WallRunComponent->GetWallJumpHorizontalVelocity(),WallRunComponent->GetWallJumpVerticalVelocity()).Length();
	WallJumpAnimationSpeed = UKismetMathLibrary::MapRangeClamped(SizeSpeed, 943.0f, 1500.0f, 1.0f,1.5f);


	float TargetHandCorrectionAdditive = CurrentArcAngle >= 15? UKismetMathLibrary::MapRangeClamped(CurrentArcAngle, 30.0f, 45.0f, 1.0f, 0.0f)
	: UKismetMathLibrary::MapRangeClamped(CurrentArcAngle, 0.0f, 30.0f, 0.0f, 1.0f);

	HandCorrectionAdditive = UKismetMathLibrary::FInterpTo(HandCorrectionAdditive, TargetHandCorrectionAdditive,DeltaSeconds, 7.0f);
	
	IKRightFootOffset = FVector((CachedPlayerCharacter->GetIKRightFootOffset() + CachedPlayerCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	IKLeftFootOffset = FVector(-(CachedPlayerCharacter->GetIKLeftFootOffset() + CachedPlayerCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	IKPelvisBoneOffset = FVector(0.0f, 0.0f, CachedPlayerCharacter->GetIKPelvisOffset());

}
