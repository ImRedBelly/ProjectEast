#include "RotatePlayerBox.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void URotatePlayerBox::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerCapture = Cast<AMainPlayerController>(GetOwningPlayer())->GetPlayerCapture();
}

FReply URotatePlayerBox::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(IsValid(PlayerCapture))
	{
		if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			float RotationDeltaZ = InMouseEvent.GetCursorDelta().X * SpeedRotation;
			PlayerCapture->AddCaptureRotation(RotationDeltaZ);
			return FReply::Handled();
		}
	}
	return FReply::Unhandled();
}

FReply URotatePlayerBox::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}
