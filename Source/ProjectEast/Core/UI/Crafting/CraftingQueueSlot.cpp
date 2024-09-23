#include "CraftingQueueSlot.h"

#include "CraftingQueue.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/CraftingCore.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingQueueSlot::InitializeSlot(UCraftingCore* InCraftingStation, FCraftingData* InCraftingData,
                                        UCraftingQueue* ParentWidget, int32 Index)
{
	CraftingStation = InCraftingStation;
	CraftingData = InCraftingData;
	ParentWidgetRef = ParentWidget;
	SlotIndex = Index;

	if (InventoryUtility::IsCraftingDataValid(CraftingData))
	{
		auto CraftableData = InventoryUtility::GetCraftableData(CraftingData);

		AmountToCraft = CraftingData->MaxCount;
		if (CraftableData.Get<0>().IsValidIndex(0))
		{
			FItemData* ItemData = CraftableData.Get<0>()[0];
			if (InventoryUtility::IsItemClassValid(ItemData))
			{
				ImageItem->SetBrushFromTexture(ItemData->Class.GetDefaultObject()->ImageItem);
			}
		}
	}


	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);
	CraftingStation->OnCraftingProcessStarted.AddDynamic(this, &UCraftingQueueSlot::OnCraftingStarted);
	ButtonCancel->OnClicked.AddDynamic(this, &UCraftingQueueSlot::OnClicked);
	ButtonMain->OnHovered.AddDynamic(this, &UCraftingQueueSlot::OnHovered);
	ButtonMain->OnUnhovered.AddDynamic(this, &UCraftingQueueSlot::OnUnhovered);

	PlayCraftingAnimation();
	SetCraftingCounterText();
}

void UCraftingQueueSlot::NativeDestruct()
{
	StopCraftingAnimation();

	if (IsValid(CraftingStation))
		CraftingStation->OnCraftingProcessStarted.RemoveDynamic(this, &UCraftingQueueSlot::OnCraftingStarted);

	ButtonCancel->OnClicked.RemoveDynamic(this, &UCraftingQueueSlot::OnClicked);
	ButtonMain->OnHovered.RemoveDynamic(this, &UCraftingQueueSlot::OnHovered);
	ButtonMain->OnUnhovered.RemoveDynamic(this, &UCraftingQueueSlot::OnUnhovered);
	Super::NativeDestruct();
}

void UCraftingQueueSlot::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	if (IconButtonGameModule->IsUsingGamepad())
	{
		SelectSlot();
		bIsFocused = true;
		SetTextColor();
	}
}

void UCraftingQueueSlot::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	if (IconButtonGameModule->IsUsingGamepad())
	{
		OnUnhovered();
	}
}

FReply UCraftingQueueSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		StopCraftingItem();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UCraftingQueueSlot::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Left)
	{
		StopCraftingItem();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UCraftingQueueSlot::OnClicked()
{
	StopCraftingItem();
}

void UCraftingQueueSlot::OnHovered()
{
	SelectSlot();
	bIsFocused = true;
	SetTextColor();
}

void UCraftingQueueSlot::OnUnhovered()
{
	PlayAnimation(HoveredAnimation, 0, 1, EUMGSequencePlayMode::Type::Reverse, 1, false);
	bIsFocused = false;
	BorderMain->SetBrushColor(FLinearColor(0, 0, 0, 0));
	SetTextColor();
}

void UCraftingQueueSlot::OnCraftingStarted(FCraftingData& Data)
{
	PlayCraftingAnimation();
}

void UCraftingQueueSlot::PlayCraftingAnimation()
{
	auto ProcessTime = CraftingStation->GetCraftingProcessTime();
	float FullTime = ProcessTime.Get<0>() + ProcessTime.Get<1>();

	float StartTime = UKismetMathLibrary::NormalizeToRange(ProcessTime.Get<0>(), 0, FullTime);
	float Speed = 1 / FMathf::Clamp(FullTime, 1, FullTime);
	PlayAnimation(CraftingAnimation, StartTime, 1, EUMGSequencePlayMode::Type::Forward, Speed);
}

void UCraftingQueueSlot::StopCraftingAnimation()
{
	StopAnimation(CraftingAnimation);
}

void UCraftingQueueSlot::SelectSlot()
{
	PlayAnimation(HoveredAnimation, 0, 1, EUMGSequencePlayMode::Type::Forward, 1, false);
	BorderMain->SetBrushColor(FLinearColor(0, 0, 0, 0));
	ParentWidgetRef->SetFocusedSlotIndex(SlotIndex);
}

void UCraftingQueueSlot::StopCraftingItem() const
{
	auto Data = CraftingStation->GetItemFromQueueByID(CraftingData->CraftingID);
	if (Data.Get<0>())
	{
		ParentWidgetRef->SetFocusedSlotIndex(SlotIndex - 1);
		CraftingStation->StopCraftingProcess(Data.Get<1>(), GetOwningPlayer());
	}
}

void UCraftingQueueSlot::SetTextColor() const
{
	FLinearColor EnableColor = FLinearColor(1, 1, 1);
	FLinearColor DisableColor = FLinearColor(.023153f, .029557f, .040915f);
	BorderCancel->SetBrushColor(bIsFocused ? EnableColor : DisableColor);
	TextCancel->SetColorAndOpacity(bIsFocused ? DisableColor : EnableColor);
}

void UCraftingQueueSlot::SetCraftingCounterText() const
{
	TextCurrentCraftingCounterAmount->SetText(GetCurrentAmount());
	TextMaxCraftingCounterAmount->SetText(GetMaxAmount());
}

FText UCraftingQueueSlot::GetCurrentAmount() const
{
	if (CraftingStation->IsCurrentlyCrafted(CraftingData))
	{
		int32 Amount = AmountToCraft - (CraftingStation->GetFirstItemFromQueue()->CraftingCounter - 1);
		return FText::FromString(FString::FromInt(Amount));
	}
	return FText::FromString(FString::FromInt(0));
}

FText UCraftingQueueSlot::GetMaxAmount() const
{
	return FText::FromString(FString::FromInt(AmountToCraft));
}
