#include "SortSlot.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USortSlot::NativePreConstruct()
{
	Super::NativePreConstruct();
	TextTitle->SetText(FText::FromName(Title));
}

void USortSlot::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonSort->OnClicked.AddDynamic(this, &USortSlot::OnClickedButton);
	ButtonSort->OnHovered.AddDynamic(this, &USortSlot::OnHoveredButton);
	ButtonSort->OnUnhovered.AddDynamic(this, &USortSlot::OnUnhoveredButton);
}

void USortSlot::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonSort->OnClicked.RemoveDynamic(this, &USortSlot::OnClickedButton);
	ButtonSort->OnHovered.RemoveDynamic(this, &USortSlot::OnHoveredButton);
	ButtonSort->OnUnhovered.RemoveDynamic(this, &USortSlot::OnUnhoveredButton);
}

void USortSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	SetBorderColor(FLinearColor(1, 1, 1));
}

void USortSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	SetBorderColor(FLinearColor());
}


FReply USortSlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	ButtonSort->SetKeyboardFocus();
	return FReply::Handled();
}

void USortSlot::OnClickedButton()
{
	if (ClickedButton.IsBound())
		ClickedButton.Broadcast(SortingMethod);
}

void USortSlot::OnHoveredButton()
{
	SetBorderColor(FLinearColor(1, 1, 1));
}

void USortSlot::OnUnhoveredButton()
{
	SetBorderColor(FLinearColor());
}

void USortSlot::SetBorderColor(FLinearColor Color) const
{
	BackgroundBorder->SetBrushColor(Color);
}
