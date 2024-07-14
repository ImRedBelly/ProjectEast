#include "GamepadControls.h"

bool IGamepadControls::IsUsingGamepad()
{
	return false;
}

void IGamepadControls::SetGamepadControls(bool IsGamepad)
{
}

AMainPlayerController* IGamepadControls::GetMainPlayerController()
{
	return nullptr;
}

EWidgetType IGamepadControls::GetCurrentlyFocusedWidget()
{
	return EWidgetType::None;
}

void IGamepadControls::SetCurrentlyFocusedWidget(EWidgetType WidgetType)
{
}
