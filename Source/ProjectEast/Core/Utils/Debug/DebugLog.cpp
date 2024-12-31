#include "DebugLog.h"
void UDebugLog::LogString(const FString& Message, float Duration, FColor Color)
{
	GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
}

void UDebugLog::LogText(const FText& Message, float Duration, FColor Color)
{
	GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message.ToString());
}
