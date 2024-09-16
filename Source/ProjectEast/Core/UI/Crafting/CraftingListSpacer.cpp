#include "CraftingListSpacer.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/Utils/GameTypes.h"

void UCraftingListSpacer::SetListTitle(const FText& TitleName) const
{
	TextTitle->SetText(TitleName);
}

void UCraftingListSpacer::NativeConstruct()
{
	Super::NativeConstruct();
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);

	ButtonList->OnClicked.AddDynamic(this, &UCraftingListSpacer::OnClickButton);
	ButtonList->OnClicked.AddDynamic(this, &UCraftingListSpacer::OnHoveredButton);
	ButtonList->OnClicked.AddDynamic(this, &UCraftingListSpacer::OnUnhoveredButton);
}

void UCraftingListSpacer::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonList->OnClicked.RemoveDynamic(this, &UCraftingListSpacer::OnClickButton);
	ButtonList->OnClicked.RemoveDynamic(this, &UCraftingListSpacer::OnHoveredButton);
	ButtonList->OnClicked.RemoveDynamic(this, &UCraftingListSpacer::OnUnhoveredButton);
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
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left ||
		InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UCraftingListSpacer::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled();
}

FReply UCraftingListSpacer::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		ToggleBoxVisibility();
		return FReply::Handled();
	}
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
		return FReply::Handled();
	return FReply::Unhandled();
}

void UCraftingListSpacer::OnClickButton()
{
	if (!IconButtonGameModule->IsUsingGamepad())
		ToggleBoxVisibility();
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
	bIsToggleVisibility = !bIsToggleVisibility;
	ImageTriangle->SetRenderTransformAngle(bIsToggleVisibility ? 0.0f : 180.0f);
	if (IsValid(AssociatedBox))
		AssociatedBox->SetVisibility(bIsToggleVisibility ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}
