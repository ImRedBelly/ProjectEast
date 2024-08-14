#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/Components/CharacterSystems/ConsumableBuffs.h"
#include "ProjectEast/Core/GameMode/InputDeviceManager.h"
#include "ConsumableBarSlot.generated.h"

UCLASS()
class PROJECTEAST_API UConsumableBarSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemSlot SlotType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush EmptySlotStyle;
	
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
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	UProgressBar* ProgressBar;
	UPROPERTY(EditAnywhere)
	UInputAction* InputAction;


	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationUse;

	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayAnimationUse(float Cooldown);
	UFUNCTION(BlueprintImplementableEvent)
	void OnAnimationUse(float Cooldown);
	
private:
	UInputDeviceManager* InputDeviceManager;
	
	UPROPERTY()
	UConsumableBuffs* CachedConsumableBuffs;
	UPROPERTY()
	UPlayerEquipment* CachePlayerEquipment;

	FString EmptySlotText = "Empty Slot";
	bool bIsUsingGamepad;

	TArray<EItemSlot> HideIfEachEmpty;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void EventRefreshValues(FItemData* ItemData);
	void EventClearValues();
	void BindOnUsed();
	void BindOnRefreshed();
	void BindOnItemChanged();
	void SetSlotVisibility();
	bool IsAnySlotValid(TArray<EItemSlot> Array) const;
	void RefreshSlot();

	UFUNCTION()
	void OnRefreshItem(FItemData ItemData);
	UFUNCTION()
	void OnItemUsed(EItemSlot UsedSLot, float Cooldown);
	UFUNCTION()
	void RefreshSlot(FItemData ItemData);
	UFUNCTION()
	void UpdateIconInput();

};
