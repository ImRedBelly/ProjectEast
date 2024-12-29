#include "OverlayStateButton.h"

void UOverlayStateButton::SetVisualParameters(bool bIsSelected) const
{
	ImageBackground->SetColorAndOpacity(bIsSelected ? BackgroundSelectedColor : BackgroundUnSelectedColor);
	TextNameOverlayState->SetColorAndOpacity(bIsSelected ? TextSelectedColor : TextUnSelectedColor);
}

void UOverlayStateButton::SetNameButton(FText NameButton) const
{
	TextNameOverlayState->SetText(NameButton);
}
