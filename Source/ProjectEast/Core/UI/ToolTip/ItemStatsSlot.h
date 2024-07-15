#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemStatsSlot.generated.h"


class UTextBlock;

UCLASS()
class PROJECTEAST_API UItemStatsSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FText StatName;
	UPROPERTY(EditAnywhere)
	float StatValue;
	UPROPERTY(EditAnywhere)
	float ComparisonValue;
	UPROPERTY(EditAnywhere)
	bool ShowComparison;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextComparisonValue1;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextComparisonValue2;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextComparisonValue3;


	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void ShowComparisonValue(float Value);
	void HideComparisonValue();
	FText GetStatValue();
	FText GetComparisonValue();
	FLinearColor GetComparisonColor();
	void SetAppropriateComparisonBox();
};
