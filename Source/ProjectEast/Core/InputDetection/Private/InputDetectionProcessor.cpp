#include "InputDetectionProcessor.h"

void InputDetectionProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

bool InputDetectionProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp,
                                                     const FAnalogInputEvent& InAnalogInputEvent)
{
	if (InAnalogInputEvent.GetKey().IsGamepadKey() && CurrentInputDevice != EHardwareDevicePrimaryType::Gamepad
		&& InAnalogInputEvent.GetAnalogValue() > 0.15f)
	{
		CurrentInputDevice = EHardwareDevicePrimaryType::Gamepad;
		OnInputDeviceChangedDelegate.ExecuteIfBound(CurrentInputDevice);
	}
	return false;
}

bool InputDetectionProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey().IsGamepadKey() && CurrentInputDevice != EHardwareDevicePrimaryType::Gamepad)
	{
		CurrentInputDevice = EHardwareDevicePrimaryType::Gamepad;
		OnInputDeviceChangedDelegate.ExecuteIfBound(CurrentInputDevice);
		return false;
	}
	if (!InKeyEvent.GetKey().IsGamepadKey() && CurrentInputDevice != EHardwareDevicePrimaryType::KeyboardAndMouse)
	{
		CurrentInputDevice = EHardwareDevicePrimaryType::KeyboardAndMouse;
		OnInputDeviceChangedDelegate.ExecuteIfBound(CurrentInputDevice);
		return false;
	}
	return false;
}

bool InputDetectionProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (CurrentInputDevice != EHardwareDevicePrimaryType::KeyboardAndMouse && MouseEvent.GetCursorDelta().Size() > 2.f)
	{
		CurrentInputDevice = EHardwareDevicePrimaryType::KeyboardAndMouse;
		OnInputDeviceChangedDelegate.ExecuteIfBound(CurrentInputDevice);
	}
	return false;
}

bool InputDetectionProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp,
                                                             const FPointerEvent& InWheelEvent,
                                                             const FPointerEvent* InGestureEvent)
{
	if (CurrentInputDevice != EHardwareDevicePrimaryType::KeyboardAndMouse)
	{
		CurrentInputDevice = EHardwareDevicePrimaryType::KeyboardAndMouse;
		OnInputDeviceChangedDelegate.ExecuteIfBound(CurrentInputDevice);
	}
	return false;
}

bool InputDetectionProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (CurrentInputDevice != EHardwareDevicePrimaryType::KeyboardAndMouse)
	{
		CurrentInputDevice = EHardwareDevicePrimaryType::KeyboardAndMouse;
		OnInputDeviceChangedDelegate.ExecuteIfBound(CurrentInputDevice);
	}
	return false;
}
