#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "UObject/NoExportTypes.h"
#include "InputDeviceManager.generated.h"


UCLASS(Blueprintable)
class PROJECTEAST_API UInputDeviceManager : public UObject
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInputDevice);
	
	GENERATED_BODY()

public:
	FOnUpdateInputDevice OnUpdateInputDevice;

	UTexture2D* GetIconKeyByActionType(UInputAction* InputAction) const;

protected:
	UPROPERTY(EditAnywhere)
	UInputMappingContext* MappingContext;
	
	UPROPERTY(EditAnywhere)
	UDataTable* DataTableKeys;	
};
