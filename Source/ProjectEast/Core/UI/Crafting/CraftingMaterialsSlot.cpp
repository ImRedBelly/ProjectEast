#include "CraftingMaterialsSlot.h"

#include "CraftingMaterialsBar.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/UI/ToolTip/ToolTip.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingMaterialsSlot::InitializeSlot(FItemData* ItemData, int32 Quantity, int32 Index,
                                            UCraftingMaterialsBar* ParentMaterialsBar)
{
	CurrentItemData = ItemData;

	CurrentQuantity = Quantity;
	SlotIndex = Index;

	if (IsValid(ParentMaterialsBar))
		ParentWidgetRef = ParentMaterialsBar;

	SetItemName();
	SetItemImage();
	SetCurrentItemAmount();
	SetNeededItemAmount();
	SetBackgroundColor();
	SetBackgroundColorText();
}

void UCraftingMaterialsSlot::NativeConstruct()
{
	Super::NativeConstruct();
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);

	ButtonItem->OnHovered.AddDynamic(this, &UCraftingMaterialsSlot::OnHovered);
	ButtonItem->OnUnhovered.AddDynamic(this, &UCraftingMaterialsSlot::OnUnhovered);
}

void UCraftingMaterialsSlot::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonItem->OnHovered.RemoveDynamic(this, &UCraftingMaterialsSlot::OnHovered);
	ButtonItem->OnUnhovered.RemoveDynamic(this, &UCraftingMaterialsSlot::OnUnhovered);
	if (IsValid(ToolTipRef))
	{
		ToolTipRef->RemoveFromParent();
		ToolTipRef = nullptr;
	}
}

void UCraftingMaterialsSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	if (IconButtonGameModule->IsUsingGamepad())
	{
		BorderItem->SetBrushColor(FLinearColor(1, 1, 1));
		if (IsValid(ParentWidgetRef))
			ParentWidgetRef->SetCurrentFocusedSlot(SlotIndex);
		RefreshToolTipWidget();
		SetTooltipPositionAndAlignment();
	}
	Super::NativeOnAddedToFocusPath(InFocusEvent);
}

void UCraftingMaterialsSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	OnUnhovered();
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

FReply UCraftingMaterialsSlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled();
}

void UCraftingMaterialsSlot::OnHovered()
{
	BorderItem->SetBrushColor(FLinearColor(1, 1, 1));
	RefreshToolTipWidget();
	SetTooltipPositionAndAlignment();
}

void UCraftingMaterialsSlot::OnUnhovered()
{
	BorderItem->SetBrushColor(FLinearColor(1, 1, 1, 0));
	if (IsValid(ToolTipRef))
	{
		ToolTipRef->RemoveFromParent();
		ToolTipRef = nullptr;
	}
}

void UCraftingMaterialsSlot::SetItemName() const
{
	TextItemName->SetText(CurrentItemData->Class.GetDefaultObject()->NameItem);
}

void UCraftingMaterialsSlot::SetItemImage() const
{
	ImageItem->SetBrushFromTexture(CurrentItemData->Class.GetDefaultObject()->ImageItem);
}

void UCraftingMaterialsSlot::SetCurrentItemAmount() const
{
	TextCurrentAmount->SetText(FText::FromString(FString::FromInt(FMathf::Clamp(CurrentQuantity, 0, CurrentQuantity))));
}

void UCraftingMaterialsSlot::SetNeededItemAmount() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
		TextAmountNeed->SetText(FText::FromString(FString::FromInt(CurrentItemData->Quantity)));
}

void UCraftingMaterialsSlot::SetBackgroundColor() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		auto Color = CurrentItemData->Quantity > CurrentQuantity ? RedColor : GreenColor;
		SlotBackgroundGradient->SetColorAndOpacity(Color);
	}
}

void UCraftingMaterialsSlot::SetBackgroundColorText() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		auto Color = CurrentItemData->Quantity > CurrentQuantity ? RedColor : GreenColor;
		TextSlash->SetColorAndOpacity(Color);
		TextAmountNeed->SetColorAndOpacity(Color);
		TextCurrentAmount->SetColorAndOpacity(Color);
	}
}

void UCraftingMaterialsSlot::SetTooltipPositionAndAlignment() const
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
		(ToolTipRef->GetDesiredSize().X * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

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
		(ToolTipRef->GetDesiredSize().Y * UWidgetLayoutLibrary::GetViewportScale(GetWorld()));

	bool VerticalValue = UKismetMathLibrary::InRange_FloatFloat(VerticalSecondPixelPosition.Y, 0.0f, VerticalMaxValue,
	                                                            true, true);

	float VerticalPosition = VerticalValue ? VerticalSecondViewportPosition.Y : VerticalFirstViewportPosition.Y;
	float VerticalAlignment = VerticalValue ? 0.0f : 1.0f;

	ToolTipRef->SetPositionInViewport(FVector2D(HorizontalPosition, VerticalPosition), false);
	ToolTipRef->SetAlignmentInViewport(FVector2D(HorizontalAlignment, VerticalAlignment));
	ToolTipRef->SetDesiredSizeInViewport(ToolTipRef->GetDesiredSize());
}

void UCraftingMaterialsSlot::RefreshToolTipWidget()
{
	if (IsValid(ToolTipRef))
	{
		ToolTipRef->RemoveFromParent();
		ToolTipRef = nullptr;
	}

	ToolTipRef = CreateWidget<UToolTip>(this, ToolTipClass);
	ToolTipRef->InitializeToolTip(CurrentItemData, false);
	ToolTipRef->AddToViewport(1);
}
