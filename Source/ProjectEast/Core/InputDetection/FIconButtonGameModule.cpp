#include "FIconButtonGameModule.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/InputSettings.h"
#include "ProjectEast/Core/InputDetection/Private/InputDetectionProcessor.h"
#include "ProjectEast/Core/InputDetection/Public/CustomDeveloperSettings.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FIconButtonGameModule, ProjectEast, "ProjectEast");


void FIconButtonGameModule::StartupModule()
{
	CurrentInputDevice = EHardwareDevicePrimaryType::KeyboardAndMouse;
	const auto DevSettings = GetDefault<UCustomDeveloperSettings>();
	DevSettings->KeyMapPtr.LoadSynchronous()->AddToRoot();
	if (FSlateApplication::IsInitialized())
	{
		InputProcessor = MakeShared<InputDetectionProcessor>();
		auto& SlateApp = FSlateApplication::Get();
		SlateApp.RegisterInputPreProcessor(InputProcessor);
		InputProcessor->OnInputDeviceChangedDelegate.BindRaw(this, &FIconButtonGameModule::OnInputDeviceChanged);
	}
}

void FIconButtonGameModule::ShutdownModule()
{
	if (FSlateApplication::IsInitialized() && InputProcessor.IsValid())
	{
		auto& SlateApp = FSlateApplication::Get();
		SlateApp.UnregisterInputPreProcessor(InputProcessor);
		InputProcessor.Reset();
	}
}

void FIconButtonGameModule::OnInputDeviceChanged(EHardwareDevicePrimaryType NewInputDevice)
{
	CurrentInputDevice = NewInputDevice;
	InputDeviceChangedDelegate.Broadcast();
	InputDeviceChanged.Broadcast();
}


TSoftObjectPtr<UTexture2D> FIconButtonGameModule::GetTextureForKey(UInputMappingContext* MappingContext,
                                                                   UInputAction* InputAction)
{
	const auto DevSettings = GetDefault<UCustomDeveloperSettings>();
	if (IsValid(MappingContext) && IsValid(InputAction))
	{
		UDataTable* KeyMap = DevSettings->KeyMapPtr.Get();
		TArray<FEnhancedActionKeyMapping> FoundMappings = MappingContext->GetMappings();

		for (const auto& FoundMapping : FoundMappings)
		{
			if (FoundMapping.Action != InputAction) continue;

			FString Context;
			FKeyToTexture* Data = KeyMap->FindRow<FKeyToTexture>(FoundMapping.Key.GetFName(), Context, true);

			if ((IsUsingGamepad() && !FoundMapping.Key.IsGamepadKey())
				|| (!IsUsingGamepad() && FoundMapping.Key.IsGamepadKey()))
					continue;

			UE_LOG(LogTemp, Warning, TEXT("FoundMapping.Key.IsGamepadKey: %s"),
			       ( FoundMapping.Key.IsGamepadKey() ? TEXT("true") : TEXT("false") ));
			UE_LOG(LogTemp, Warning, TEXT("IsUsingGamepad: %s"), ( IsUsingGamepad() ? TEXT("true") : TEXT("false") ));

			if (!Data || (Data->Key != FoundMapping.Key))continue;

			if (Data && Context.IsEmpty())
				return Data->Texture;
		}
	}
	return nullptr;
}
