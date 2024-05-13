#include "ExampleTileline.h"

// void APlayerCharacter::OnCameraArmChange(float Value) const
// {
// 	//	SpringArmComponent->TargetArmLength = StartCameraTargetArmLength + (OffsetCameraTargetArmLength * Value);
// }

//BEGINPLAY()
// {
// 	
// 	// FOnTimelineFloat OnTimelineCallback;
// 	// OnTimelineCallback.BindUFunction(this, FName{TEXT("OnCameraArmChange")});
// 	//
// 	// CameraArmChangeTimeline.SetTimelineLength(0.5f);
// 	// CameraArmChangeTimeline.SetTimelineLengthMode(TL_TimelineLength);
// 	// CameraArmChangeTimeline.AddInterpFloat(CameraTargetArmUpdateCurve, OnTimelineCallback);
// }

//StartCameraTargetArmLength = SpringArmComponent->TargetArmLength;
//	OffsetCameraTargetArmLength = CrouchAppendCameraTargetArmLength;

// if (StartCameraTargetArmLength > NormalCameraTargetArmLength)
// 	OffsetCameraTargetArmLength = CrouchAppendCameraTargetArmLength - (StartCameraTargetArmLength -
// 		NormalCameraTargetArmLength);
//
// CameraArmChangeTimeline.Play();


// FTimeline CameraArmChangeTimeline;
// float TimelineValue;
// float CurveFloatValue;



//	OffsetCameraTargetArmLength = CrouchAppendCameraTargetArmLength;
//	CameraArmChangeTimeline.Reverse();



//OffsetCameraTargetArmLength = SprintAppendCameraTargetArmLength;
//CameraArmChangeTimeline.Reverse();



// StartCameraTargetArmLength = SpringArmComponent->TargetArmLength;
// OffsetCameraTargetArmLength = SprintAppendCameraTargetArmLength;
//
// if (StartCameraTargetArmLength < NormalCameraTargetArmLength)
// 	OffsetCameraTargetArmLength = SprintAppendCameraTargetArmLength + (NormalCameraTargetArmLength -
// 		StartCameraTargetArmLength);
//
// CameraArmChangeTimeline.Play();


// void APlayerCharacter::Tick(float DeltaSeconds)
// {
// 	Super::Tick(DeltaSeconds);
// 	CameraArmChangeTimeline.TickTimeline(DeltaSeconds);
//
// }