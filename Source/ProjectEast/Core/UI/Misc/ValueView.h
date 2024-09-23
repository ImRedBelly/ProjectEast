#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ValueView.generated.h"

UCLASS()
class PROJECTEAST_API UValueView : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetValue(float Value) const;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextValue;
};
