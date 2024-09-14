#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Engine/DataTable.h"
#include "GameFramework/InputSettings.h"
#include "Modules/ModuleManager.h"
#include "FIconButtonGameModule.generated.h"

DECLARE_MULTICAST_DELEGATE(FChangeButtonLabelsDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputDeviceChanged);

USTRUCT()
struct FKeyToTexture : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FKey Key;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Texture;
};

class InputDetectionProcessor;

class FIconButtonGameModule : public IModuleInterface
{
protected:
	TSharedPtr<InputDetectionProcessor> InputProcessor;

	EHardwareDevicePrimaryType CurrentInputDevice;

public:
	FChangeButtonLabelsDelegate InputDeviceChangedDelegate;
	FInputDeviceChanged InputDeviceChanged;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool IsGameModule() const override
	{
		return true;
	}

	bool IsUsingGamepad() const;
	void OnInputDeviceChanged(EHardwareDevicePrimaryType NewInputDevice);
	TSoftObjectPtr<UTexture2D> GetTextureForKey(UInputMappingContext* MappingContext, UInputAction* InputAction);
};
