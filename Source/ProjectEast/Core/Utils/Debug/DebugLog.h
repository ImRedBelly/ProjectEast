#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugLog.generated.h"

UCLASS()
class PROJECTEAST_API UDebugLog : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static void LogString(const FString& Message, float Duration = 1.0f, FColor Color = FColor::Red);
	static void LogText(const FText& Message, float Duration = 1.0f, FColor Color = FColor::Red);
};
