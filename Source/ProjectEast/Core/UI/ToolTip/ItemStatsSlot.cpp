#include "ItemStatsSlot.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetTextLibrary.h"

void UItemStatsSlot::InitializeSlot(FString NameState, float ValueStat,
	float ValueComparison, bool ComparisonShow)
{
	StatName = NameState;
	StatValue = ValueStat;
	ComparisonValue = ValueComparison;
	ShowComparison = ComparisonShow;
}

void UItemStatsSlot::ShowComparisonValue(float Value)
{
	ComparisonValue = Value;
	ShowComparison = true;
	UpdateViewSwitcher();
}

void UItemStatsSlot::HideComparisonValue()
{
	ShowComparison = false;
	UpdateViewSwitcher();
}
void UItemStatsSlot::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsDesignTime())
	{
		TextStatName->SetText(FText::FromString(StatName));
		TextStatValue->SetText(FText::FromString(FString::SanitizeFloat(StatValue)));
		TextComparisonValue1->SetText(FText::FromString(FString::SanitizeFloat(ComparisonValue)));
	}
}

void UItemStatsSlot::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateViewSwitcher();
}


FText UItemStatsSlot::GetStatValue() const
{
	return UKismetTextLibrary::Conv_DoubleToText(StatValue, HalfToEven,
	                                             false, true, 1, 324, 0, 2);
}

FText UItemStatsSlot::GetComparisonValue() const
{
	return UKismetTextLibrary::Conv_DoubleToText(StatValue - ComparisonValue, HalfToEven,
	                                             false, true, 1, 324, 0, 2);
}

FLinearColor UItemStatsSlot::GetComparisonColor() const
{
	if (StatValue - ComparisonValue == 0.0f)
		return FLinearColor(0.442708f, 0.442708f, 0.442708f);

	return FLinearColor();
}

void UItemStatsSlot::SetAppropriateComparisonBox() const
{
	float Value = StatValue - ComparisonValue;
	
	if (Value > 0.0f)
		WidgetSwitcher->SetActiveWidgetIndex(0);
	else if (Value < 0.0f)
		WidgetSwitcher->SetActiveWidgetIndex(1);
	else if (Value == 0.0f)
		WidgetSwitcher->SetActiveWidgetIndex(2);
}

void UItemStatsSlot::UpdateViewSwitcher() const
{
	WidgetSwitcher->
		SetVisibility(ShowComparison ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

	if (ShowComparison)
		SetAppropriateComparisonBox();
}
