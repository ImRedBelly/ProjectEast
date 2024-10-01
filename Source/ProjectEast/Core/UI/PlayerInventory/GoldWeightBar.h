#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GoldWeightBar.generated.h"

class UPlayerInventory;

UCLASS()
class PROJECTEAST_API UGoldWeightBar : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextGold;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextCurrentWeight;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextMaximumWeight;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextWeightSpacer;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* GoldAnimation;

private:
	UPROPERTY()
	UPlayerInventory* PlayerInventory;
	
	FLinearColor WhiteColor = FLinearColor(1,1,1);
	FLinearColor RedColor = FLinearColor(.337164f, .001821f, .001821f);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindEventDispatchers();
	void UnbindEventDispatchers();

	UFUNCTION()
	void SetOwnerGold();
	UFUNCTION()
	void SetCurrentWeight();
	UFUNCTION()
	void SetMaximumWeight();
};
