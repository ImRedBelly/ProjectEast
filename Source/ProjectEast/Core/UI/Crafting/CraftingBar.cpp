#include "CraftingBar.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingBar::InitializeBar(UPlayerCrafting* InPlayerCrafting, UCraftingCore* InCraftingStation)
{
	PlayerCrafting = InPlayerCrafting;
	CraftingStation = InCraftingStation;

	SliderValue->SetValue(0);
	ProgressBarSelectAmount->SetPercent(0);
	TextValue->SetText(FText::FromString(FString::FromInt(1)));

	BindEventDispatchers();
}

void UCraftingBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime())
		WidgetSwitcherIsLocked->SetActiveWidgetIndex(ActiveIndex);
}

void UCraftingBar::NativeDestruct()
{
	UnbindEventDispatchers();
	Super::NativeDestruct();
}

FReply UCraftingBar::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled();
}

void UCraftingBar::BindEventDispatchers()
{
	PlayerCrafting->OnNewItemSelected.AddDynamic(this, &UCraftingBar::OnItemSelected);
	CraftingStation->OnRefreshed.AddDynamic(this, &UCraftingBar::RefreshWidgetData);
	CraftingStation->OnCraftingProcessStarted.AddDynamic(this, &UCraftingBar::OnCraftingStarted);
	CraftingStation->OnCraftingProcessStopped.AddDynamic(this, &UCraftingBar::OnCraftingStopped);
	CraftingStation->OnAddedToQueue.AddDynamic(this, &UCraftingBar::OnAddedToCraftingQueue);
	CraftingStation->OnRemoveFromQueue.AddDynamic(this, &UCraftingBar::OnRemovedFromCraftingQueue);

	ButtonCraft->OnClicked.AddDynamic(this, &UCraftingBar::OnClickedCraftingButton);
	ButtonCraft->OnHovered.AddDynamic(this, &UCraftingBar::OnHoveredCraftingButton);
	ButtonCraft->OnUnhovered.AddDynamic(this, &UCraftingBar::OnUnhoveredCraftingButton);

	SliderValue->OnValueChanged.AddDynamic(this, &UCraftingBar::OnValueChangedSlider);
}

void UCraftingBar::UnbindEventDispatchers()
{
	PlayerCrafting->OnNewItemSelected.RemoveDynamic(this, &UCraftingBar::OnItemSelected);
	CraftingStation->OnRefreshed.RemoveDynamic(this, &UCraftingBar::RefreshWidgetData);
	CraftingStation->OnCraftingProcessStarted.RemoveDynamic(this, &UCraftingBar::OnCraftingStarted);
	CraftingStation->OnCraftingProcessStopped.RemoveDynamic(this, &UCraftingBar::OnCraftingStopped);
	CraftingStation->OnAddedToQueue.RemoveDynamic(this, &UCraftingBar::OnAddedToCraftingQueue);
	CraftingStation->OnRemoveFromQueue.RemoveDynamic(this, &UCraftingBar::OnRemovedFromCraftingQueue);

	ButtonCraft->OnClicked.RemoveDynamic(this, &UCraftingBar::OnClickedCraftingButton);
	ButtonCraft->OnHovered.RemoveDynamic(this, &UCraftingBar::OnHoveredCraftingButton);
	ButtonCraft->OnUnhovered.RemoveDynamic(this, &UCraftingBar::OnUnhoveredCraftingButton);

	SliderValue->OnValueChanged.RemoveDynamic(this, &UCraftingBar::OnValueChangedSlider);
}

void UCraftingBar::OnClickedCraftingButton()
{
	if (CraftingStation->IsCurrentlyCrafted(StoredCraftingData))
		StopCrafting();
	else
		AttemptCrafting();
}

void UCraftingBar::OnHoveredCraftingButton()
{
	ImageCraftBackground->SetColorAndOpacity(HoveredColorBackground);
}

void UCraftingBar::OnUnhoveredCraftingButton()
{
	ImageCraftBackground->SetColorAndOpacity(UnhoveredColorBackground);
}

void UCraftingBar::OnValueChangedSlider(float Value)
{
	if (CanChangeSliderValue())
	{
		ValueSlider = Value;
		ProgressBarSelectAmount->SetPercent(ValueSlider);

		SetCurrentCraftableAmount();
		SetCraftingCost();
	}
}

void UCraftingBar::OnItemSelected(FCraftingData& CraftingData)
{
	CurrentAmount = 1;
	ValueSlider = 0;
	RefreshWidgetData();
}

void UCraftingBar::RefreshWidgetData()
{
	FCraftingData* SelectedCraftingData = PlayerCrafting->GetSelectedCraftingData();
	if (InventoryUtility::IsCraftingDataValid(SelectedCraftingData))
	{
		StoredCraftingData = SelectedCraftingData;

		bool IsCraftingRecipeLocked = PlayerCrafting->IsCraftingRecipeLocked(StoredCraftingData);
		WidgetSwitcherIsLocked->SetActiveWidgetIndex(IsCraftingRecipeLocked ? 1 : 0);
		
		if (CraftingStation->IsCrafting() && CraftingStation->IsCurrentlyCrafted(StoredCraftingData))
		{
			auto TimeData = CraftingStation->GetCraftingProcessTime();
			PlayCraftingAnimation(TimeData.Get<0>(), TimeData.Get<0>() + TimeData.Get<1>());
			BorderCraftingCounter->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SetCraftableItemImage();
			SetCraftableItemName();
			SetCraftingCounterText();
			CurrentAmount = 1;
			SetMaxCraftableAmount();
			CanvasSliderBox->SetIsEnabled(false);
		}
		else
		{
			StopAnimation(CraftingAnimation);

			bool IsCraftableItemInQueue = PlayerCrafting->IsCraftableItemInQueue(StoredCraftingData);
			WidgetSwitcherButtonContent->SetActiveWidgetIndex(IsCraftableItemInQueue ? 2 : 0);

			ProgressBarCrafting->SetPercent(0);
			SliderValue->SetIsEnabled(true);
			BorderCraftingCounter->SetVisibility(ESlateVisibility::Collapsed);
			CanvasSliderBox->SetIsEnabled(!CraftingStation->IsCraftableItemInQueue(StoredCraftingData));
			SetMaxCraftableAmount();
			SetCurrentCraftableAmount();
			SetCraftingCost();
		}
	}
	ForceLayoutPrepass();
}

void UCraftingBar::OnCraftingStarted(FCraftingData& CraftingData)
{
	if (InventoryUtility::AreCraftingDatasTheSame(CraftingData, StoredCraftingData))
	{
		CraftingProcessStarted();
	}
}

void UCraftingBar::OnCraftingStopped(FCraftingData& CraftingData)
{
	if (InventoryUtility::AreCraftingDatasTheSame(CraftingData, StoredCraftingData))
	{
		CraftingProcessStopped();
	}
	else
	{
		SetMaxCraftableAmount();
	}
}

void UCraftingBar::OnAddedToCraftingQueue(FCraftingData& CraftingData, int QueueIndex)
{
	CurrentAmount = 1;
	RefreshWidgetData();
}

void UCraftingBar::OnRemovedFromCraftingQueue(FCraftingData& CraftingData, int QueueIndex)
{
	RefreshWidgetData();
}

void UCraftingBar::CraftingProcessStarted()
{
	PlayCraftingAnimation(0, StoredCraftingData->CraftingTime);
	RefreshWidgetData();

	auto CraftingData = GetCraftingData();
	if (CraftingData->CraftingCounter == CraftingData->MaxCount)
	{
		CurrentAmount = 1;
		SetMaxCraftableAmount();
	}
}

void UCraftingBar::CraftingProcessStopped()
{
	StopAnimation(CraftingAnimation);
	SliderValue->SetIsEnabled(true);
	CurrentAmount = 1;
	RefreshWidgetData();
}

void UCraftingBar::StopCrafting() const
{
	if (CraftingStation->IsCrafting())
	{
		CraftingStation->StopCraftingProcess(CraftingStation->GetFirstItemFromQueue(), GetOwningPlayer());
	}
}

void UCraftingBar::AttemptCrafting()
{
	if (CanStartCrafting())
	{
		PlayerCrafting->ServerInitializeCraftingProcess(StoredCraftingData, CurrentAmount, CraftingStation,
		                                                GetOwningPlayer());
	}
}


void UCraftingBar::PlayCraftingAnimation(float PlayAtTime, float CraftingDuration)
{
	float StartTime = UKismetMathLibrary::NormalizeToRange(PlayAtTime, 0, CraftingDuration);
	float Speed = 1 / FMathf::Clamp(CraftingDuration, 1, CraftingDuration);
	PlayAnimation(CraftingAnimation, StartTime, 1, EUMGSequencePlayMode::Type::Forward, Speed);
	WidgetSwitcherButtonContent->SetActiveWidgetIndex(1);
}

void UCraftingBar::SetCurrentCraftableAmount()
{
	CurrentAmount = UKismetMathLibrary::MapRangeClamped(ValueSlider, 0, 1, 1, MaxAmount);
	TextValue->SetText(FText::FromString(FString::FromInt(CurrentAmount)));
}

void UCraftingBar::SetMaxCraftableAmount()
{
	int32 LocalMin = 1;
	int32 LocalMax = 999;

	auto CraftableData = InventoryUtility::GetCraftableData(StoredCraftingData);
	for (auto Data : CraftableData.Get<1>())
	{
		if (IsValid(Data.TableAndRow.DataTable))
		{
			FItemData* ItemData = Data.TableAndRow.DataTable->FindRow<FItemData>(Data.TableAndRow.RowName, TEXT(""));
			int32 QuantityFound = PlayerCrafting->FindItemQuantity(ItemData, GetOwningPlayer());

			int32 Delta = QuantityFound / Data.Quantity;

			if (Delta < LocalMin)
			{
				auto ClampValue = FMathf::Clamp(Delta, 1, Delta);
				LocalMin = ClampValue;
				LocalMax = ClampValue;
			}
			else
			{
				if (Delta < LocalMax)
				{
					LocalMax = FMathf::Clamp(Delta, 1, Delta);
				}
			}
		}
	}

	MaxAmount = LocalMax;
	TextMaxAmount->SetText(FText::FromString(FString::FromInt(MaxAmount)));
}

void UCraftingBar::SetCraftableItemName() const
{
	auto CraftableData = InventoryUtility::GetCraftableData(StoredCraftingData);
	auto Data = CraftableData.Get<0>()[0];
	if (InventoryUtility::IsItemClassValid(Data))
	{
		TextItemName->SetText(Data->Class.GetDefaultObject()->NameItem);
	}
}

void UCraftingBar::SetCraftingCost() const
{
	float Cost = CurrentAmount * (StoredCraftingData->CraftingCost * CraftingStation->GetCraftingCostMultiplier());
	FText TextCost = UKismetTextLibrary::Conv_DoubleToText(Cost, HalfToEven, false, true,
	                                                       1.0f, 324.0f, 0.0, 0.0);
	TextCraftingCost->SetText(TextCost);


	FLinearColor Color = CraftingStation->HasPlayerEnoughGold(StoredCraftingData, CurrentAmount, GetOwningPlayer())
		                     ? ColorTextEnoughGold
		                     : ColorTextNoEnoughGold;

	TextCraftingCost->SetColorAndOpacity(Color);
}

void UCraftingBar::SetCraftableItemImage() const
{
	auto CraftableData = InventoryUtility::GetCraftableData(StoredCraftingData);
	auto Data = CraftableData.Get<0>()[0];
	if (InventoryUtility::IsItemClassValid(Data))
	{
		ImageCraftableItem->SetBrushFromTexture(Data->Class.GetDefaultObject()->ImageItem);
	}
}

void UCraftingBar::SetCraftingCounterText() const
{
	TextCurrentCraftingCounterAmount->SetText(GetCraftingCounterText());
	TextMaxCraftingCounterAmount->SetText(GetCraftingMaxCounterText());
}

FText UCraftingBar::GetCraftingCounterText() const
{
	if (CraftingStation->IsCurrentlyCrafted(StoredCraftingData))
	{
		auto Data = CraftingStation->GetFirstItemFromQueue();
		auto Counter = Data->MaxCount - (Data->CraftingCounter - 1);
		return FText::FromString(FString::FromInt(Counter));
	}
	return FText::FromString("0");
}

FText UCraftingBar::GetCraftingMaxCounterText() const
{
	if (CraftingStation->IsCurrentlyCrafted(StoredCraftingData))
	{
		auto Data = CraftingStation->GetFirstItemFromQueue();
		return FText::FromString(FString::FromInt(Data->MaxCount));
	}
	return FText::FromString("0");
}

FCraftingData* UCraftingBar::GetCraftingData() const
{
	FCraftingData* Item = CraftingStation->GetItemFromQueueByID(StoredCraftingData->CraftingID).Get<1>();
	if (InventoryUtility::IsCraftingDataValid(Item))
		return Item;
	return StoredCraftingData;
}

bool UCraftingBar::CanChangeSliderValue() const
{
	return !CraftingStation->IsCurrentlyCrafted(StoredCraftingData);
}

bool UCraftingBar::CanStartCrafting()
{
	return !CraftingStation->IsCraftableItemInQueue(StoredCraftingData) &&
		!PlayerCrafting->IsCraftableItemInQueue(StoredCraftingData);
}
