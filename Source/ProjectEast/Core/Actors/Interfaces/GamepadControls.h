#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "UObject/Interface.h"
#include "GamepadControls.generated.h"

class AMainPlayerController;

UINTERFACE(MinimalAPI)
class UGamepadControls : public UInterface
{
	GENERATED_BODY()
};

class PROJECTEAST_API IGamepadControls
{
	GENERATED_BODY()

public:
	virtual bool IsUsingGamepad();
	virtual void SetGamepadControls(bool IsGamepad);
	virtual AMainPlayerController* GetMainPlayerController();

	virtual EWidgetType GetCurrentlyFocusedWidget();
	virtual void SetCurrentlyFocusedWidget(EWidgetType WidgetType);
};
