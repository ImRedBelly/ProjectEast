#include "CraftingListSlot.h"
#include "CraftingListBox.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/UI/ToolTip/ToolTip.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingListSlot::InitializeSlot(FCraftingData* InCraftingData, UPlayerCrafting* InPlayerCrafting)
{
	CraftingData = InCraftingData;
	PlayerCrafting = InPlayerCrafting;
}

void UCraftingListSlot::SetSlotIndex(UCraftingListBox* InCraftingListBox, int32 Index)
{
	CraftingListBox = InCraftingListBox;
	CurrentIndex = Index;
}

void UCraftingListSlot::SelectSlot()
{
	bIsSelected = true;
	PlayerCrafting->FocusSelectedItem(CraftingData);
	CraftingListBox->OnSlotSelected(this);
	ButtonItem->SetStyle(SelectedStyle);
	SetTextColor();
	PlayAnimation(FocusAnim);
}

void UCraftingListSlot::UnselectSlot()
{
	bIsSelected = false;
	ButtonItem->SetStyle(UnselectedStyle);
	SetTextColor();
	PlayAnimation(FocusAnim, 0.0f, 1, EUMGSequencePlayMode::Type::Reverse);
}

void UCraftingListSlot::NativeConstruct()
{
	Super::NativeConstruct();
	WidgetManager = Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager();

	SetItemData();
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);

	ButtonItem->OnClicked.AddDynamic(this, &UCraftingListSlot::OnClicked);
	ButtonItem->OnHovered.AddDynamic(this, &UCraftingListSlot::OnHovered);
	ButtonItem->OnUnhovered.AddDynamic(this, &UCraftingListSlot::OnUnhovered);
}

void UCraftingListSlot::NativeDestruct()
{
	Super::NativeDestruct();
	if (IsValid(CachedToolTip))
	{
		CachedToolTip->RemoveFromParent();
		CachedToolTip = nullptr;
	}

	ButtonItem->OnClicked.RemoveDynamic(this, &UCraftingListSlot::OnClicked);
	ButtonItem->OnHovered.RemoveDynamic(this, &UCraftingListSlot::OnHovered);
	ButtonItem->OnUnhovered.RemoveDynamic(this, &UCraftingListSlot::OnUnhovered);
}

void UCraftingListSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	if (IconButtonGameModule->IsUsingGamepad())
	{
		CraftingListBox->OnSlotFocused(this);
		ImageSlotBackground->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
		WidgetManager->SetCurrentlyFocusedWidget(EWidgetType::PlayerCrafting);
		AttemptSelectingSlot();
	}
}

void UCraftingListSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	if (IconButtonGameModule->IsUsingGamepad())
	{
		WidgetManager->SetCurrentlyFocusedWidget(EWidgetType::None);
		ButtonItem->SetToolTip(nullptr);
		ImageSlotBackground->SetColorAndOpacity(FLinearColor(1, 1, 1));
		if (IsValid(CachedToolTip))
		{
			CachedToolTip->RemoveFromParent();
			CachedToolTip = nullptr;
		}
	}
}

FReply UCraftingListSlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled();
}

FReply UCraftingListSlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		ShowItemComparison();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UCraftingListSlot::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		HideItemComparison();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UCraftingListSlot::AttemptSelectingSlot()
{
	if (!InventoryUtility::AreItemsTheSame(ItemData,
	                                       PlayerCrafting->GetCurrentCraftableData().Get<0>()))
	{
		InventoryUtility::PlaySoundOnItemDropped();
		SelectSlot();
	}
}

void UCraftingListSlot::ShowItemComparison() const
{
	if (InventoryUtility::IsItemClassValid(ItemData)
		&& ItemData->Class.GetDefaultObject()->Rarity != EItemRarity::Consumable
		&& IsValid(CachedToolTip))
	{
		CachedToolTip->ShowComparisonToolTip();
	}
}

void UCraftingListSlot::HideItemComparison() const
{
	if (IsValid(CachedToolTip))
	{
		CachedToolTip->HideComparisonToolTip();
	}
}

void UCraftingListSlot::SetToolTipPositionAndAlignment()
{
	FVector2D HorizontalFirstPixelPosition;
	FVector2D HorizontalFirstViewportPosition;
	FVector2D HorizontalSecondPixelPosition;
	FVector2D HorizontalSecondViewportPosition;

	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(), GetCachedGeometry().GetLocalSize(),
	                                        HorizontalFirstPixelPosition, HorizontalFirstViewportPosition);
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),
	                                        FVector2D(0, GetCachedGeometry().GetLocalSize().Y),
	                                        HorizontalSecondPixelPosition, HorizontalSecondViewportPosition);

	float HorizontalMaxValue = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).X -
		(CachedToolTip->GetDesiredSize().X * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

	bool HorizontalValue = UKismetMathLibrary::InRange_FloatFloat(HorizontalSecondPixelPosition.X, 0.0f,
	                                                              HorizontalMaxValue, true, true);

	float HorizontalPosition = HorizontalValue ? HorizontalSecondViewportPosition.X : HorizontalFirstViewportPosition.X;
	float HorizontalAlignment = HorizontalValue ? 0.0f : 1.0f;


	FVector2D VerticalFirstPixelPosition;
	FVector2D VerticalFirstViewportPosition;
	FVector2D VerticalSecondPixelPosition;
	FVector2D VerticalSecondViewportPosition;

	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(),
	                                        FVector2D(GetCachedGeometry().GetLocalSize().X, 0),
	                                        VerticalFirstPixelPosition, VerticalFirstViewportPosition);
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(), GetCachedGeometry().GetLocalSize(),
	                                        VerticalSecondPixelPosition, VerticalSecondViewportPosition);

	float VerticalMaxValue = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).Y -
		(CachedToolTip->GetDesiredSize().Y * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

	bool VerticalValue = UKismetMathLibrary::InRange_FloatFloat(VerticalSecondPixelPosition.Y, 0.0f, VerticalMaxValue,
	                                                            true, true);

	float VerticalPosition = VerticalValue ? VerticalSecondViewportPosition.Y : VerticalFirstViewportPosition.Y;
	float VerticalAlignment = VerticalValue ? 0.0f : 1.0f;

	CachedToolTip->SetPositionInViewport(FVector2D(HorizontalPosition, VerticalPosition), false);
	CachedToolTip->SetAlignmentInViewport(FVector2D(HorizontalAlignment, VerticalAlignment));
	CachedToolTip->SetDesiredSizeInViewport(CachedToolTip->GetDesiredSize());
}

void UCraftingListSlot::SetItemData()
{
	ItemData = InventoryUtility::GetCraftableData(CraftingData).Get<0>()[0];
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		ImageLock->SetVisibility(PlayerCrafting->IsCraftingRecipeLocked(CraftingData)
			                         ? ESlateVisibility::Visible
			                         : ESlateVisibility::Hidden);
		SetItemImage();
		SetItemName();
		SetItemType();
		SetItemRarity();
		SetItemRarityColor();
	}
}

void UCraftingListSlot::RefreshGeometryCache()
{
	if (ButtonItem->HasKeyboardFocus())
	{
		if (IsValid(CachedToolTip))
		{
			CachedToolTip->RemoveFromParent();
			CachedToolTip = nullptr;
		}

		CachedToolTip = CreateWidget<UToolTip>(this, ToolTipClass);
		CachedToolTip->InitializeToolTip(ItemData, false);
		CachedToolTip->AddToViewport(1);
		SetToolTipPositionAndAlignment();
	}
}

void UCraftingListSlot::SetItemImage() const
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		ImageItem->SetBrushFromTexture(ItemData->Class.GetDefaultObject()->ImageItem);
	}
}

void UCraftingListSlot::SetItemName() const
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		TextItemName->SetText(ItemData->Class.GetDefaultObject()->NameItem);
	}
}

void UCraftingListSlot::SetItemType() const
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		TextItemType->SetText(UEnum::GetDisplayValueAsText(ItemData->Class.GetDefaultObject()->Type));
	}
}

void UCraftingListSlot::SetItemRarity() const
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		TextItemRarity->SetText(UEnum::GetDisplayValueAsText(ItemData->Class.GetDefaultObject()->Rarity));
	}
}

void UCraftingListSlot::SetItemRarityColor() const
{
	if (InventoryUtility::IsItemClassValid(ItemData))
	{
		auto Color = InventoryUtility::GetRarityColor(ItemData->Class.GetDefaultObject()->Rarity);
		ImageRarityLabel->SetColorAndOpacity(Color.GetSpecifiedColor());
	}
}

void UCraftingListSlot::SetTextColor() const
{
	auto Color = bIsSelected ? FLinearColor(.017684f, .022575f, .03125f) : FLinearColor(1, 1, 1);

	TextItemName->SetColorAndOpacity(Color);
	TextItemType->SetColorAndOpacity(Color);
	TextItemRarity->SetColorAndOpacity(Color);
}

void UCraftingListSlot::OnClicked()
{
	if (!IconButtonGameModule->IsUsingGamepad())
	{
		AttemptSelectingSlot();
	}
}

void UCraftingListSlot::OnHovered()
{
	if (!IconButtonGameModule->IsUsingGamepad())
	{
		if (IsValid(CachedToolTip))
		{
			CachedToolTip->RemoveFromParent();
			CachedToolTip = nullptr;
		}

		SetKeyboardFocus();
		ImageSlotBackground->SetColorAndOpacity(FLinearColor(1, 1, 1));

		CachedToolTip = CreateWidget<UToolTip>(this, ToolTipClass);
		CachedToolTip->InitializeToolTip(ItemData, false);
		ButtonItem->SetToolTip(CachedToolTip);
	}
}

void UCraftingListSlot::OnUnhovered()
{
	WidgetManager->SetCurrentlyFocusedWidget(EWidgetType::None);
	ButtonItem->SetToolTip(nullptr);
	ImageSlotBackground->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
	if (IsValid(CachedToolTip))
	{
		CachedToolTip->RemoveFromParent();
		CachedToolTip = nullptr;
	}
}
