#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "SplitStackPopup.generated.h"

class UValueView;
class UWeightView;
class UPlayerInventory;
class UInventoryCore;
class AMainPlayerController;

UCLASS()
class PROJECTEAST_API USplitStackPopup : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializePopup(FItemData* ItemData, FItemData* InSlotData,
	UInventoryCore* Sender, UInventoryCore* Receiver, EInputMethodType InputMethod,
	EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget);
protected:
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderBackground;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* QuantityText;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MaxQuantityText;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonAccept;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonCancel;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBar;
	UPROPERTY(meta=(BindWidget))
	USlider* SliderObject;
	UPROPERTY(meta=(BindWidget))
	UWeightView* WeightView;
	UPROPERTY(meta=(BindWidget))
	UValueView* ValueView;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationConstruct;


	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnChangeSliderValue(float Value);
	UFUNCTION()
	void OnTextChange();
	UFUNCTION()
	void OnClickAccept();
	UFUNCTION()
	void OnClickCancel();
	UFUNCTION()
	void OnPressedAccept();
	UFUNCTION()
	void OnPressedCancel() const;
	UFUNCTION()
	void OnWidgetSwitched(EWidgetType WidgetType);

private:
	AMainPlayerController* CachedController;
	FIconButtonGameModule* IconButtonGameModule;

	UInventoryCore* CachedSender;
	UInventoryCore* CachedReceiver;
	UPlayerInventory* PlayerInventory;
	UUserWidget* CachedSenderWidget;

	FItemData* CurrentItemData;
	FItemData* InSlotItemData;
	FItemData* StackableLeftItemData;

	EWidgetType RecentlyFocusedWidget;
	EInputMethodType CachedInputMethod;
	EItemDestination CachedInitiator;
	EItemDestination CachedDestination;

	float SliderValue;
	bool bISGamepadControls;

	void SetCurrentWeight() const;
	void SetCurrentValue() const;
	void ModifyCurrentQuantity(uint32 ValueToAdd);

	float GetSliderValue() const;
	FText GetItemQuantityText() const;
	FSlateColor SetValueColor() const;

	uint32 GetCurrentQuantity() const;
	bool IsUsingGamepad() const;
};
