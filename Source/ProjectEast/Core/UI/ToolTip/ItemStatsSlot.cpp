#include "ItemStatsSlot.h"

void UItemStatsSlot::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UItemStatsSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemStatsSlot::ShowComparisonValue(float Value)
{
}

void UItemStatsSlot::HideComparisonValue()
{
}

FText UItemStatsSlot::GetStatValue()
{
	return FText();
}

FText UItemStatsSlot::GetComparisonValue()
{
	return FText();
}

FLinearColor UItemStatsSlot::GetComparisonColor()
{
	return FLinearColor();
}

void UItemStatsSlot::SetAppropriateComparisonBox()
{
}
