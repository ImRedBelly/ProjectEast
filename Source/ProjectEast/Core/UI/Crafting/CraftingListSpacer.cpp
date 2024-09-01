#include "CraftingListSpacer.h"

void UCraftingListSpacer::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetListTitle(TitleName);
}

void UCraftingListSpacer::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCraftingListSpacer::NativeDestruct()
{
	Super::NativeDestruct();
}

void UCraftingListSpacer::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	OnHoveredButton();
}

void UCraftingListSpacer::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	OnUnhoveredButton();
}

FReply UCraftingListSpacer::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UCraftingListSpacer::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

FReply UCraftingListSpacer::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UCraftingListSpacer::OnClickButton()
{
}

void UCraftingListSpacer::OnHoveredButton()
{
	ImageBackground->SetColorAndOpacity(ColorActive);
}

void UCraftingListSpacer::OnUnhoveredButton()
{
	ImageBackground->SetColorAndOpacity(ColorInactive);
}

void UCraftingListSpacer::ToggleBoxVisibility()
{
}

void UCraftingListSpacer::SetListTitle(FText Title)
{
	TitleName = Title;
	TextTitle->SetText(TitleName);
}

bool UCraftingListSpacer::IsUsingGamepad()
{
	return false;
}
