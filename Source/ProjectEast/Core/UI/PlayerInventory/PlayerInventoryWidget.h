#pragma once

#include "CoreMinimal.h"
#include "PlayerInventorySlot.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventoryWidget.generated.h"


UCLASS()
class PROJECTEAST_API UPlayerInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AssignCurrentlyFocusedSlot(UPlayerInventorySlot* PlayerInventorySlot);
	void ScrollToSlot(UPlayerInventorySlot* PlayerInventorySlot);
};
