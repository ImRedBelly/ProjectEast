#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "PopupItemAdded.generated.h"

UCLASS()
class PROJECTEAST_API UPopupItemAdded : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWidget(FItemData* ItemData);

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageRarityBlock;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemType;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemRarity;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayConstructAnimation();

private:
	FItemData* CurrentItemData;

	void SetItemImage() const;
	void SetItemName() const;
	void SetItemType() const;
	void SetItemRarity() const;
	void SetItemRarityColor() const;
};
