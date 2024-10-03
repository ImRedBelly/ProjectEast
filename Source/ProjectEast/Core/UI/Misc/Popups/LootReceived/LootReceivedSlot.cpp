#include "LootReceivedSlot.h"

#include "PopupLootReceived.h"

void ULootReceivedSlot::InitializeSlot(UPopupLootReceived* InParentWidget, FText ItemName, int32 ItemQuantity)
{
	ParentWidget = InParentWidget;
	PlayAnimation(ConstructAnimation);

	TextItemName->SetText(ItemName);

	FText TextQuantity = ItemQuantity > 1 ? FText::FromString(FString::FromInt(ItemQuantity)) : FText();
	TextItemQuantity->SetText(TextQuantity);
}

void ULootReceivedSlot::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	ParentWidget->RemoveLootedItem(this);
}