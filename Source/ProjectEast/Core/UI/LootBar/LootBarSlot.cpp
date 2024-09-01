#include "LootBarSlot.h"
#include "LootBar.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void ULootBarSlot::InitializeSlot(ULootBar* LootBar, FItemData* ItemData)
{
	CachedLootBar = LootBar;
	CachedItemData = ItemData;

	SetIsFocusable(true);
	SetImageItem();
	SetItemName();
	SetItemType();
	SetItemRarity();
	SetItemRarityColor();
	SetItemQuantity();
}

void ULootBarSlot::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);
	BackgroundButton->OnClicked.AddUniqueDynamic(this, &ULootBarSlot::OnRightClick);
	BackgroundButton->OnHovered.AddUniqueDynamic(this, &ULootBarSlot::OnHovered);
}

FReply ULootBarSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClick();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply ULootBarSlot::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetCursorDelta().X != 0.0f || InMouseEvent.GetCursorDelta().Y != 0.0f)
	{
		CachedLootBar->IsKeyboardFocus = false;
		OnMouseMoved();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void ULootBarSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	ClearAnimation();
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

void ULootBarSlot::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	ClearAnimation();
	Super::NativeOnFocusLost(InFocusEvent);
}

void ULootBarSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);

	if (IsUsingGamepad() || CachedLootBar->IsKeyboardFocus)
	{
		BorderImage->SetColorAndOpacity(FLinearColor(1, 1, 1));
		CachedLootBar->SetCurrentlyFocusedSlot(this);
		CachedLootBar->ScrollWidget(this);
		PlayAnimation(AnimationBorder, 0.0f, 0.0f, EUMGSequencePlayMode::Forward, 1.0f, false);
	}
}

void ULootBarSlot::SetItemName() const
{
	if (InventoryUtility::IsItemClassValid(CachedItemData))
		TextItemName->SetText(CachedItemData->Class.GetDefaultObject()->NameItem);
}

void ULootBarSlot::SetItemType() const
{
	if (InventoryUtility::IsItemClassValid(CachedItemData))
		TextItemType->SetText(UEnum::GetDisplayValueAsText(CachedItemData->Class.GetDefaultObject()->Type));
}

void ULootBarSlot::SetItemRarity() const
{
	if (InventoryUtility::IsItemClassValid(CachedItemData))
		TextItemRarity->SetText(UEnum::GetDisplayValueAsText(CachedItemData->Class.GetDefaultObject()->Rarity));
}

void ULootBarSlot::SetItemRarityColor() const
{
	if (InventoryUtility::IsItemClassValid(CachedItemData))
		TextItemRarity->SetColorAndOpacity(
			InventoryUtility::GetRarityColor(CachedItemData->Class.GetDefaultObject()->Rarity));
}

void ULootBarSlot::SetItemQuantity() const
{
	if (InventoryUtility::IsItemClassValid(CachedItemData))
	{
		if (CachedItemData->Class.GetDefaultObject()->bIsStackable && CachedItemData->Quantity > 1)
		{
			FText Value = UKismetTextLibrary::Conv_IntToText(CachedItemData->Quantity,
			false, true, 1, 324);

			TextItemQuantity->SetText(Value);
		}
		else
			TextItemQuantity->SetText(FText());
	}
}

void ULootBarSlot::SetImageItem() const
{
	if (InventoryUtility::IsItemClassValid(CachedItemData))
		ImageItem->SetBrushFromTexture(CachedItemData->Class.GetDefaultObject()->ImageItem);
}

void ULootBarSlot::OnRightClick()
{
	CachedLootBar->TakeItem();
}

void ULootBarSlot::OnMouseMoved()
{
	if (!HasKeyboardFocus())
		SetActivateState();
}

void ULootBarSlot::OnHovered()
{
	if (!IsUsingGamepad() && !CachedLootBar->IsKeyboardFocus)
		SetActivateState();
}

bool ULootBarSlot::IsUsingGamepad() const
{
	return IconButtonGameModule->IsUsingGamepad();
}

void ULootBarSlot::SetActivateState()
{
	SetKeyboardFocus();
	BorderImage->SetColorAndOpacity(FLinearColor(1, 1, 1));
	CachedLootBar->SetCurrentlyFocusedSlot(this);
	PlayAnimation(AnimationBorder, 0.0f, 0.0f, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void ULootBarSlot::ClearAnimation()
{
	StopAnimation(AnimationBorder);
	BorderImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
}
