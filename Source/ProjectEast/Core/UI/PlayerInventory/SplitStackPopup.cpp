#include "SplitStackPopup.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void USplitStackPopup::InitializePopup(FItemData* ItemData, FItemData* InSlotData, UInventoryCore* Sender,
	UInventoryCore* Receiver, EInputMethodType Method, EItemDestination Initiator, EItemDestination Destination,
	UUserWidget* SenderWidget)
{
	CurrentItemData= ItemData;
	InSlotItemData = InSlotData;
	CachedSender = Sender;
	CachedReceiver =Receiver;
	CachedInputMethod = Method;
	CachedInitiator = Initiator;
	CachedDestination = Destination;
	CachedSenderWidget = SenderWidget;
}

void USplitStackPopup::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerInventory = InventoryUtility::GetPlayerInventory(GetOwningPlayer());
	InventoryUtility::PlaySoundOnItemDropped();
	
	CachedController = Cast<AMainPlayerController>(GetOwningPlayer());
	if (IsValid(CachedController))
		CachedController->OnSwitchedWidget.AddDynamic(this, &USplitStackPopup::OnWidgetSwitched);
	PlayAnimation(AnimationConstruct, 0.0f, 1,EUMGSequencePlayMode::Forward,1.0f, false);
	
	MaxQuantityText->SetText(FText::FromString(FString::FromInt(CurrentItemData->Quantity)));
	OnChangeSliderValue(1);
	SliderObject->SetValue(1);

	ButtonAccept->OnClicked.AddDynamic(this, &USplitStackPopup::OnClickAccept);
	ButtonCancel->OnClicked.AddDynamic(this, &USplitStackPopup::OnClickCancel);
	SliderObject->OnValueChanged.AddDynamic(this, &USplitStackPopup::OnChangeSliderValue);
}

void USplitStackPopup::NativeDestruct()
{
	Super::NativeDestruct();
	if (IsValid(CachedController))
		CachedController->OnSwitchedWidget.RemoveDynamic(this, &USplitStackPopup::OnWidgetSwitched);
	
	ButtonAccept->OnClicked.RemoveDynamic(this, &USplitStackPopup::OnClickAccept);
	ButtonCancel->OnClicked.RemoveDynamic(this, &USplitStackPopup::OnClickCancel);
	SliderObject->OnValueChanged.RemoveDynamic(this, &USplitStackPopup::OnChangeSliderValue);
}

FReply USplitStackPopup::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled();
}

FReply USplitStackPopup::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply USplitStackPopup::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void USplitStackPopup::OnChangeSliderValue(float Value)
{
	SliderValue = Value;
	ProgressBar->SetPercent(Value);
	SetCurrentWeight();
	SetCurrentValue();
	
	QuantityText->SetText(FText::FromString(FString::FromInt(GetCurrentQuantity())));
}

void USplitStackPopup::OnTextChange()
{
	SliderValue = UKismetMathLibrary::Clamp(UKismetMathLibrary::NormalizeToRange(GetCurrentQuantity(), 1.0f,
	CurrentItemData->Quantity), 0.0f, 1.0f);
	SetCurrentWeight();
	SetCurrentValue();
}

void USplitStackPopup::OnClickAccept()
{
	OnPressedAccept();
}

void USplitStackPopup::OnClickCancel()
{
	OnPressedCancel();
}

void USplitStackPopup::OnPressedAccept()
{
	StackableLeftItemData = InventoryUtility::CopyItemData(CurrentItemData);
	StackableLeftItemData->Quantity = CurrentItemData->Quantity - GetCurrentQuantity();
	CurrentItemData->Quantity = GetCurrentQuantity();
	OnPressedCancel();

	PlayerInventory->ServerSplitItemsInInventory(CachedReceiver, CachedSender, CurrentItemData, InSlotItemData, StackableLeftItemData,
		CachedInputMethod, CachedInitiator, CachedDestination, GetOwningPlayer());
}

void USplitStackPopup::OnPressedCancel() const
{
	if(IsValid(CachedController))
		CachedController->GetWidgetManager()->CloseActivePopup();
	if(IsValid(CachedSenderWidget))
		CachedSenderWidget->SetKeyboardFocus();
}

void USplitStackPopup::OnWidgetSwitched(EWidgetType WidgetType)
{
	OnPressedCancel();
}

void USplitStackPopup::SetCurrentWeight() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		WeightView->SetValue(CurrentItemData->Class.GetDefaultObject()->Weight * GetCurrentQuantity());
	}
}

void USplitStackPopup::SetCurrentValue() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		ValueView->SetValue(
			(CurrentItemData->Class.GetDefaultObject()->Value * CurrentItemData->ValueModifier) * GetCurrentQuantity());
	}
}

void USplitStackPopup::ModifyCurrentQuantity(uint32 ValueToAdd)
{
	SliderValue = UKismetMathLibrary::NormalizeToRange(GetCurrentQuantity() + ValueToAdd, 1.0f,
	                                                   CurrentItemData->Quantity);
	SetCurrentWeight();
	SetCurrentValue();
}


float USplitStackPopup::GetSliderValue() const
{
	return SliderValue;
}

FText USplitStackPopup::GetItemQuantityText() const
{
	return FText::FromString(FString::FromInt(GetCurrentQuantity()));
}

FSlateColor USplitStackPopup::SetValueColor() const
{
	if (IsValid(CachedReceiver) && CachedReceiver->CheckOwnerGold() && IsValid(CachedSender) && CachedSender->CheckOwnerGold())
	{
		auto CurrentValue = CurrentItemData->Class.GetDefaultObject()->Value * CurrentItemData->ValueModifier *
			GetCurrentQuantity();
		if (CachedReceiver->GetOwnerGold() >= CurrentValue)
			return FSlateColor(FLinearColor(1, 1, 1));

		return FSlateColor(FLinearColor(0.526f, 0, .01f));
	}
	return FSlateColor(FLinearColor(1, 1, 1));
}

uint32 USplitStackPopup::GetCurrentQuantity() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		return UKismetMathLibrary::Round(
			UKismetMathLibrary::MapRangeClamped(
				SliderValue, 0.0f, 1.0f, 1.0f, CurrentItemData->Quantity));
	}
	return 0;
}

bool USplitStackPopup::IsUsingGamepad() const
{
	if (IsValid(CachedController))
		return CachedController->IsUsingGamepad();
	return false;
}
