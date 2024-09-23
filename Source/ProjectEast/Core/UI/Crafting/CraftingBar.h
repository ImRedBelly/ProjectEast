#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingBar.generated.h"


class UCraftingCore;
class UPlayerCrafting;

UCLASS()
class PROJECTEAST_API UCraftingBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeBar(UPlayerCrafting* InPlayerCrafting, UCraftingCore* InCraftingStation);

protected:
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderBackground;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderCraftingCounter;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonCraft;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* CanvasSliderBox;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageCraftBackground;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageCraftableItem;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextValue;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextCraftingCost;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextItemName;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextMaxAmount;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBarSelectAmount;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBarCrafting;
	UPROPERTY(meta=(BindWidget))
	USlider* SliderValue;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcherButtonContent;
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcherIsLocked;

	UPROPERTY(EditAnywhere)
	FLinearColor HoveredColorBackground;
	UPROPERTY(EditAnywhere)
	FLinearColor UnhoveredColorBackground;
	UPROPERTY(EditAnywhere)
	FLinearColor ColorTextEnoughGold;
	UPROPERTY(EditAnywhere)
	FLinearColor ColorTextNoEnoughGold;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* CraftingAnimation;

private:
	UPROPERTY()
	UPlayerCrafting* PlayerCrafting;
	UPROPERTY()
	UCraftingCore* CraftingStation;


	FCraftingData* StoredCraftingData;
	int32 CurrentAmount;
	float ValueSlider;
	float MaxAmount;
	float ActiveIndex;
	float Time;

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void BindEventDispatchers();
	void UnbindEventDispatchers();

	UFUNCTION()
	void OnClickedCraftingButton();
	UFUNCTION()
	void OnHoveredCraftingButton();
	UFUNCTION()
	void OnUnhoveredCraftingButton();
	UFUNCTION()
	void OnValueChangedSlider(float Value);
	UFUNCTION()
	void OnItemSelected(FCraftingData& CraftingData);
	UFUNCTION()
	void RefreshWidgetData();
	UFUNCTION()
	void OnCraftingStarted(FCraftingData& CraftingData);
	UFUNCTION()
	void OnCraftingStopped(FCraftingData& CraftingData);
	UFUNCTION()
	void OnAddedToCraftingQueue(FCraftingData& CraftingData, int QueueIndex);
	UFUNCTION()
	void OnRemovedFromCraftingQueue(FCraftingData& CraftingData, int QueueIndex);

	void CraftingProcessStarted();
	void CraftingProcessStopped();

	void StopCrafting() const;
	void AttemptCrafting();
	void PlayCraftingAnimation(float PlayAtTime, float CraftingDuration);

	void SetCurrentCraftableAmount();
	void SetMaxCraftableAmount();
	void SetCraftableItemName() const;
	void SetCraftingCost() const;
	void SetCraftableItemImage() const;
	//void ModifySliderValue(float Value);

	//FText GetCraftingCounterText() const;
	//FText GetCraftingMaxCounterText() const;
	FCraftingData* GetCraftingData() const;
	bool CanChangeSliderValue() const;
	bool CanStartCrafting();
};
