#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PopupMessage.generated.h"

UCLASS()
class PROJECTEAST_API UPopupMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	void DisplayMessage(FText Message);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextMessage;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ShowAnimation;
	
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
};
