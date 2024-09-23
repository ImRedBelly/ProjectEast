#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "WeightView.generated.h"

UCLASS()
class PROJECTEAST_API UWeightView : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetValue(float Weight) const;

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextValue;
};
