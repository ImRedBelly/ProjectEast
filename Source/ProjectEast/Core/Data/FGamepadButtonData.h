#pragma once

#include "CoreMinimal.h"  
#include "Engine/DataTable.h"  
#include "FGamepadButtonData.generated.h"


UENUM(BlueprintType)
enum class EGamepadButtonType : uint8
{
	FaceButtonBottom UMETA(DisplayName = "FaceButtonBottom"),
	FaceButtonRight UMETA(DisplayName = "FaceButtonRight"),
	FaceButtonLeft UMETA(DisplayName = "FaceButtonLeft"),
	FaceButtonTop UMETA(DisplayName = "FaceButtonTop"),
	LeftThumbstick UMETA(DisplayName = "LeftThumbstick"),
	LeftThumbstickButton UMETA(DisplayName = "LeftThumbstickButton"),
	RightThumbstick UMETA(DisplayName = "RightThumbstick"),
	RightThumbstickButton UMETA(DisplayName = "RightThumbstickButton"),
	LeftShoulder UMETA(DisplayName = "LeftShoulder"),
	RightShoulder UMETA(DisplayName = "RightShoulder"),
	LeftTrigger UMETA(DisplayName = "LeftTrigger"),
	RightTrigger UMETA(DisplayName = "RightTrigger"),
	DpadUp UMETA(DisplayName = "DpadUp"),
	DpadDown UMETA(DisplayName = "DpadDown"),
	DpadRight UMETA(DisplayName = "DpadRight"),
	DpadLeft UMETA(DisplayName = "DpadLeft"),
	SpecialLeft UMETA(DisplayName = "SpecialLeft"),
	SpecialRight UMETA(DisplayName = "SpecialRight"),
};

USTRUCT(BlueprintType)
struct FGamepadButtonData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	EGamepadButtonType InputType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	UTexture2D* InputIcon;
};

