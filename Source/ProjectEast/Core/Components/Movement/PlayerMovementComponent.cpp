#include "PlayerMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

void UPlayerMovementComponent::BeginPlay()
{
	FOnTimelineFloat OnTimelineCallback;
	OnTimelineCallback.BindUFunction(this, FName{TEXT("OnCameraArmChange")});

	CameraArmChangeTimeline.SetTimelineLength(0.5f);
	CameraArmChangeTimeline.AddInterpFloat(CameraTargetArmUpdateCurve, OnTimelineCallback);
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CameraArmChangeTimeline.TickTimeline(DeltaTime);
}

void UPlayerMovementComponent::Initialize(USpringArmComponent* Component)
{
	SpringArmComponent = Component;
}

float UPlayerMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
		Result = SprintSpeed;
	return Result;
}

void UPlayerMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
	CameraArmChangeTimeline.PlayFromStart();
}

void UPlayerMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
	CameraArmChangeTimeline.ReverseFromEnd();
}

void UPlayerMovementComponent::OnCameraArmChange(float Value) const
{
	SpringArmComponent->TargetArmLength = 300 + (170 * Value);
}
