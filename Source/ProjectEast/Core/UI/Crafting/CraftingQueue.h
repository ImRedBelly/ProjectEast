#pragma once

#include "CoreMinimal.h"
#include "CraftingBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "CraftingQueue.generated.h"


UCLASS()
class PROJECTEAST_API UCraftingQueue : public UUserWidget
{
	GENERATED_BODY()

public:
	bool CanBeFocused() const;
	void SetWidgetToTransferFocus(UUserWidget* UserWidget);

protected:
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* HorizontalBoxSlots;


private:
	UPROPERTY()
	UUserWidget* WidgetToTransferFocus;
};
