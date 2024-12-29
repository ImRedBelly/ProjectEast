#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "OverlayStateButton.generated.h"


UCLASS()
class PROJECTEAST_API UOverlayStateButton : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetVisualParameters(bool bIsSelected) const;
	void SetNameButton(FText NameButton) const;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextNameOverlayState;

	UPROPERTY(EditAnywhere)
	FLinearColor BackgroundSelectedColor;
	UPROPERTY(EditAnywhere)
	FLinearColor BackgroundUnSelectedColor;
	UPROPERTY(EditAnywhere)
	FLinearColor TextSelectedColor;
	UPROPERTY(EditAnywhere)
	FLinearColor TextUnSelectedColor;
};
