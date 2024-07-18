#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpperUIBar.generated.h"

class UButton;

UCLASS()
class PROJECTEAST_API UUpperUIBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonClose;

virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void CloseInventory();
};
