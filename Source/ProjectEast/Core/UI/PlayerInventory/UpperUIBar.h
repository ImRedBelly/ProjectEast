#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "UpperUIBar.generated.h"


UCLASS()
class PROJECTEAST_API UUpperUIBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonClose;
	
	UPROPERTY(EditAnywhere)
	EWidgetType CloseWidgetType;


private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UFUNCTION()
	void CloseWidget();
};
