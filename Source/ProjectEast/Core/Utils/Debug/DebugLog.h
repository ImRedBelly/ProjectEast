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

	template <typename EnumType>
	FORCEINLINE static void LogEnum(const EnumType EnumeratorValue, const FString& Message, float Duration = 1.0f,
	                                FColor Color = FColor::Red)
	{
		static_assert(TIsEnum<EnumType>::Value, "Should only call this with enum types");
		UEnum* EnumClass = StaticEnum<EnumType>();
		check(EnumClass != nullptr);

		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message + 
		                                 EnumClass->GetDisplayNameTextByValue((int64)EnumeratorValue).ToString());
	}
};
