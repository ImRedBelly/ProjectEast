#pragma once

#include "CoreMinimal.h"
#include "InputDeviceManager.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectEastGameMode.generated.h"

UCLASS()
class PROJECTEAST_API AProjectEastGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UInputDeviceManager* GetInputDeviceManager() const { return InputDeviceManager.GetDefaultObject(); }

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInputDeviceManager> InputDeviceManager;

	virtual void BeginPlay() override;
};
