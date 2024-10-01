#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "LootReceivedSlot.generated.h"


class UPopupLootReceived;

UCLASS()
class PROJECTEAST_API ULootReceivedSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(UPopupLootReceived* InParentWidget, FText ItemName, int32 ItemQuantity);

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemQuantity;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ConstructAnimation;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

private:
	UPROPERTY()
	UPopupLootReceived* ParentWidget;
};
