#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemStatsSlot.generated.h"


class UWidgetSwitcher;
class UTextBlock;

UCLASS()
class PROJECTEAST_API UItemStatsSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeSlot(FText NameState, float ValueStat, float ValueComparison, bool ComparisonShow);

	void ShowComparisonValue(float Value);
	void HideComparisonValue();

	float GetStatValue() {return StatValue;}
	
protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextStatName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextStatValue;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextComparisonValue1;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextComparisonValue2;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextComparisonValue3;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcher;
	
	virtual void NativeConstruct() override;
	
	FText GetStatValue() const;
	FText GetComparisonValue() const;
	FLinearColor GetComparisonColor() const;
	void SetAppropriateComparisonBox() const;

	void UpdateViewSwitcher() const;

private:
	float StatValue;
	float ComparisonValue;
	bool ShowComparison;
};
