#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventorySlot.generated.h"

class UButton;

UCLASS()
class PROJECTEAST_API UPlayerInventorySlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonItem;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnClickedButtonItem();
	void OnDropKeyPressed();

	
};
