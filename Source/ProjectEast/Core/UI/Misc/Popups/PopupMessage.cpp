#include "PopupMessage.h"

void UPopupMessage::DisplayMessage(FText Message)
{
	TextMessage->SetText(Message);
	PlayAnimation(ShowAnimation);
}

void UPopupMessage::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	RemoveFromParent();
}
