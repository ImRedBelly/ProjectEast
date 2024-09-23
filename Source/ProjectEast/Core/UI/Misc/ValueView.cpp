#include "ValueView.h"

void UValueView::SetValue(float Value) const
{
	TextValue->SetText(FText::FromString(FString::SanitizeFloat(Value)));
}
