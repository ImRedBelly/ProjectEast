#include "ValueView.h"

void UValueView::SetItemData(FItemData* ItemData)
{
}

void UValueView::SetValue(float Value)
{
	TextValue->SetText(FText::FromString(FString::SanitizeFloat(Value)));
}
