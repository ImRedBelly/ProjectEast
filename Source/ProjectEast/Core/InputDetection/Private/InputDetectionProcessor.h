#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "GameFramework/InputSettings.h"

DECLARE_DELEGATE_OneParam(FOnInputDeviceChangedDelegate, EHardwareDevicePrimaryType);

class InputDetectionProcessor : public IInputProcessor
{
public:
	FOnInputDeviceChangedDelegate OnInputDeviceChangedDelegate;

	void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

	virtual bool
	HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent,
	                                            const FPointerEvent* InGestureEvent) override;

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

private:
	EHardwareDevicePrimaryType CurrentInputDevice = EHardwareDevicePrimaryType::KeyboardAndMouse;
};
