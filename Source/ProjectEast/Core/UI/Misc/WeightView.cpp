#include "WeightView.h"

void UWeightView::SetValue(float Weight) const
{
	TextValue->SetText(FText::FromString(FString::SanitizeFloat(Weight)));
}
