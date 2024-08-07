#include "WeightView.h"

void UWeightView::SetItemData(FItemData* ItemData)
{
}

void UWeightView::SetValue(float Weight)
{
	TextValue->SetText(FText::FromString(FString::SanitizeFloat(Weight)));
}
