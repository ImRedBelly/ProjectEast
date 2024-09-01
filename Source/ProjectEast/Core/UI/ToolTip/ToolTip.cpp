#include "ToolTip.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/KismetTextLibrary.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"
#include "ProjectEast/Core/Components/PlayerLeveling.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"

void UToolTip::InitializeToolTip(FItemData* ItemData, bool IsCompareItems)
{
	CurrentItemData = ItemData;
	bIsCompareItems = IsCompareItems;
}

void UToolTip::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerEquipment = InventoryUtility::GetPlayerEquipment(GetOwningPlayer());
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		BorderEquipped->SetVisibility(bIsCompareItems
			                              ? ESlateVisibility::SelfHitTestInvisible
			                              : ESlateVisibility::Hidden);
		PlayAnimation(ToolTipAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
		SetItemInfo();
		ForceLayoutPrepass();
	}
	else
		SetVisibility(ESlateVisibility::Hidden);
}

void UToolTip::ShowComparisonToolTip()
{
	if (IsValid(ComparisonToolTip))
	{
		auto Data = PlayerEquipment->GetItemByEquipmentSlot(CurrentItemData->EquipmentSlot);
		if (Data.Get<0>())
		{
			auto NewToolTip = CreateWidget<UToolTip>(this, DefaultToolTip);
			NewToolTip->InitializeToolTip(Data.Get<1>(), true);
			ComparisonToolTip = NewToolTip;
			auto GridSlot = UniformGrid->AddChildToUniformGrid(ComparisonToolTip, 0, 0);

			GridSlot->SetColumn(-1);
			GridSlot->SetHorizontalAlignment(HAlign_Right);

			UpdateItemStatsComparison(ComparisonToolTip);
			ComparisonToolTip->UpdateItemStatsComparison(this);
		}
	}
}

void UToolTip::HideComparisonToolTip()
{
	if (IsValid(ComparisonToolTip))
	{
		ComparisonToolTip->RemoveFromParent();
		ComparisonToolTip = nullptr;
		UpdateItemStatsComparison(nullptr);
	}
}

void UToolTip::SetItemInfo()
{
	SetItemName();
	SetItemType();
	SetItemRarity();
	SetItemImage();
	SetItemDescription();
	SetItemRarityColor();
	BuildItemStats();
	SetItemWeight();
	SetItemValue();
	SetItemRequiredLevel();
	SetItemRequiredLevelColor();
	SetItemDurability();
	SetItemDurabilityColor();
	SetAlreadyReadVisibility();
}

void UToolTip::SetItemName() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		TextItemName->SetText(CurrentItemData->Class.GetDefaultObject()->NameItem);
	}
}

void UToolTip::SetItemType() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		TextItemType->SetText(
			FText::FromString(UEnum::GetValueAsString(CurrentItemData->Class.GetDefaultObject()->Type)));
	}
}

void UToolTip::SetItemRarity() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		TextItemRarity->SetText(
			FText::FromString(UEnum::GetValueAsString(CurrentItemData->Class.GetDefaultObject()->Rarity)));
	}
}

void UToolTip::SetItemImage() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		ItemIcon->SetBrushFromTexture(CurrentItemData->Class.GetDefaultObject()->ImageItem);
	}
}


void UToolTip::SetItemDescription() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		auto Description = CurrentItemData->Class.GetDefaultObject()->Description;
		if (UKismetTextLibrary::TextIsEmpty(Description))
			TextItemDescription->SetVisibility(ESlateVisibility::Collapsed);
		else
		{
			TextItemDescription->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TextItemDescription->SetText(Description);
		}
	}
}

void UToolTip::SetItemRarityColor() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		auto Color = InventoryUtility::GetRarityColor(CurrentItemData->Class.GetDefaultObject()->Rarity);
		BorderItemInfo->SetContentColorAndOpacity(Color.GetSpecifiedColor());
		BorderItemInfo->SetBrushColor(Color.GetSpecifiedColor());
	}
}

void UToolTip::BuildItemStats()
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		StatWidgets.Empty();
		VerticalBoxItemStats->ClearChildren();
		for (auto ItemsStat : CurrentItemData->Class.GetDefaultObject()->Stats)
		{
			auto NewItemStatsSlot = CreateWidget<UItemStatsSlot>(this, DefaultItemStatsSlot);

			FString StatName = UEnum::GetValueAsString(ItemsStat.Type);
			NewItemStatsSlot->InitializeSlot(StatName, ItemsStat.Value, 0.0f, false);

			StatWidgets.Add(StatName, NewItemStatsSlot);
			auto VerticalBoxSlot = VerticalBoxItemStats->AddChildToVerticalBox(NewItemStatsSlot);
			VerticalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
		}
	}
}

void UToolTip::SetItemWeight() const
{
	WeightView->SetItemData(CurrentItemData);
}

void UToolTip::SetItemValue() const
{
	ValueView->SetItemData(CurrentItemData);
}

void UToolTip::SetItemRequiredLevel() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		uint32 RequiredLevel = CurrentItemData->Class.GetDefaultObject()->RequiredLevel;
		if (RequiredLevel > 0)
		{
			TextRequiredLevel->SetText(FText::FromString(FString::FromInt(RequiredLevel)));
			HorizontalBoxRequiredLevel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
			HorizontalBoxRequiredLevel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UToolTip::SetItemRequiredLevelColor() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		UPlayerLeveling* ActorLeveling = Cast<UPlayerLeveling>(
			GetOwningPlayer()->GetComponentByClass(UPlayerLeveling::StaticClass()));

		if (IsValid(ActorLeveling))
		{
			FLinearColor Color = CurrentItemData->Class.GetDefaultObject()->RequiredLevel <=
			                     ActorLeveling->GetCurrentLevel()
				                     ? FLinearColor::Green
				                     : FLinearColor::Red;
			TextRequiredLevel->SetColorAndOpacity(Color);
			TextRequiredLevelValue->SetColorAndOpacity(Color);
		}
	}
}

void UToolTip::SetItemDurability() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		if (CurrentItemData->Class.GetDefaultObject()->bUseDurability)
		{
			uint32 Durability = CurrentItemData->Durability;
			FString DurabilityString = FString::Printf(TEXT("%d%%"), Durability);
			TextRequiredLevel->SetText(FText::FromString(DurabilityString));
			HorizontalBoxDurability->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
			HorizontalBoxDurability->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UToolTip::SetItemDurabilityColor() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		FLinearColor ColorText = CurrentItemData->Durability > 10
			                         ? FLinearColor::Green : FLinearColor::Red;
		TextDurability->SetColorAndOpacity(ColorText);
		TextDurabilityValue->SetColorAndOpacity(ColorText);
	}
}

void UToolTip::UpdateItemStatsComparison(UToolTip* ComparedToolTip)
{
	if (IsValid(ComparedToolTip))
	{
		for (auto KV : StatWidgets)
		{
			float StatValue = 0.0f;
			if (ComparisonToolTip->GetStatWidgets().Contains(KV.Key))
			{
				auto StatsSlot = *ComparisonToolTip->GetStatWidgets().Find(KV.Key);
				StatValue = StatsSlot->GetStatValue();
			}

			KV.Value->ShowComparisonValue(StatValue);
		}
	}
	else
	{
		for (auto KV : StatWidgets)
			KV.Value->HideComparisonValue();
	}
}

void UToolTip::SetAlreadyReadVisibility() const
{
	if (InventoryUtility::IsItemClassValid(CurrentItemData))
	{
		auto bIsVisibleText = CurrentItemData->Class.GetDefaultObject()->UseType == EItemUseType::TextDocument &&
			CurrentItemData->bIsAlreadyUsed;

		TextAlreadyRead->SetVisibility(bIsVisibleText
			                               ? ESlateVisibility::SelfHitTestInvisible
			                               : ESlateVisibility::Collapsed);
	}
}
