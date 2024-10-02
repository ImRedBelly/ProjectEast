#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "PopupGreaterLoot.generated.h"


class UPlayerInventory;

UCLASS()
class PROJECTEAST_API UPopupGreaterLoot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializePopup(UPlayerInventory* InPlayerInventory);

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemMessage;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemQuantity;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ConstructAnimation;

private:
	UPROPERTY()
	UPlayerInventory* PlayerInventory;

	TArray<FItemData*> LootedItems;

	virtual void NativeDestruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	UFUNCTION()
	void OnItemLooted(FItemData& ItemData);
	void DisplayNewItem();

	void SetViewItemData(FItemData* ItemData);
	bool IsEligibleToDisplay(FItemData* ItemData);
	FText GetTextMessageBasedOnItem(FItemData* ItemData);
};
