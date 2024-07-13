#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "UObject/Interface.h"
#include "GamepadControls.generated.h"

UINTERFACE(MinimalAPI)
class UGamepadControls : public UInterface
{
	GENERATED_BODY()
};

class PROJECTEAST_API IGamepadControls
{
	GENERATED_BODY()

public:
	EWidgetType GetCurrentlyFocusedWidget();
	void SetCurrentlyFocusedWidget(EWidgetType WidgetType);
	bool IsGamepad;
};
