#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/Components/CharacterSystems/ConsumableBuffs.h"
#include "ConsumableBarSlot.generated.h"

UCLASS()
class PROJECTEAST_API UConsumableBarSlot : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	UImage* ImageInput;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageItem;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemQuantity;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextX;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush EmptySlotStyle;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationUse;

	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UConsumableBuffs* CachedConsumableBuffs;
	UPROPERTY()
	UPlayerEquipment* CachePlayerEquipment;

	FText EmptySlotText;
	EItemSlot SlotType;
	bool bIsUsingGamepad;

	TArray<EItemSlot> HideIfEachEmpty;

	void EventRefreshValues();
	void EventClearValues();
	void BindOnUsed();
	void BindOnRefreshed();
	void BindOnItemChanged();
	void OnItemUsed();
	void OnRefreshItem();
	void RefreshSlot();
	void GamepadToggled();
	void PlayCooldownAnimation();
	void SetSlotVisibility();
	bool IsAnySlotValid();
	bool IsUsingGamepad();
};
