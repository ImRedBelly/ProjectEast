#include "CraftingBase.h"

#include "CraftingBar.h"
#include "CraftingMaterialsBar.h"
#include "CraftingQueue.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/KismetTextLibrary.h"
#include "ProjectEast/Core/Components/PlayerLeveling.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/Components/Equipment/PlayerEquipment.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/UI/Misc/ValueView.h"
#include "ProjectEast/Core/UI/Misc/WeightView.h"
#include "ProjectEast/Core/UI/ToolTip/ItemStatsSlot.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UCraftingBase::InitializeCraftingBase(UPlayerCrafting* InPlayerCrafting, UCraftingCore* InCraftingCore)
{
	PlayerCrafting = InPlayerCrafting;
	CraftingStation = InCraftingCore;
	
	auto OwningPlayer = GetOwningPlayer();
	PlayerEquipment = InventoryUtility::GetPlayerEquipment(OwningPlayer);
	PlayerLeveling = InventoryUtility::GetPlayerLeveling(OwningPlayer);
	
	PlayerCrafting->OnRefreshed.AddDynamic(this, &UCraftingBase::RefreshCraftingData);
	CraftingStation->OnRefreshed.AddDynamic(this, &UCraftingBase::RefreshCraftingData);

	CraftingMaterialsBar->InitializeWidget(PlayerCrafting, CraftingStation);
	PlayAnimation(ItemInfoAnimation);
	WidgetSwitcherCraftingBox->SetActiveWidgetIndex(CraftingStation->GetCanCraftItems() ? 0 : 1);

	ButtonClearQueue->OnClicked.AddDynamic(this, &UCraftingBase::OnClicked);
	CraftingQueue->InitializeQueue(CraftingStation);
	CraftingBar->InitializeBar(PlayerCrafting, CraftingStation);
}

void UCraftingBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsValid(QueueFocusTransfer))
		CraftingQueue->SetWidgetToTransferFocus(QueueFocusTransfer);
}

void UCraftingBase::NativeDestruct()
{
	PlayerCrafting->OnRefreshed.RemoveDynamic(this, &UCraftingBase::RefreshCraftingData);
	CraftingStation->OnRefreshed.RemoveDynamic(this, &UCraftingBase::RefreshCraftingData);
	ButtonClearQueue->OnClicked.RemoveDynamic(this, &UCraftingBase::OnClicked);
	Super::NativeDestruct();
}

FReply UCraftingBase::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	CraftingMaterialsBar->SetKeyboardFocus();
	return FReply::Handled();
}

void UCraftingBase::OnClicked()
{
	CraftingStation->ClearCraftingQueue(GetOwningPlayer());
}

void UCraftingBase::RefreshCraftingData()
{
	ForceLayoutPrepass();
	SetCurrentCraftingData();
	SetItemName();
	SetItemType();
	SetItemRarity();
	SetItemImage();
	SetItemDescription();
	SetItemRarityColor();
	BuildStatsSlots();
	SetItemWeight();
	SetItemValue();
	SetItemRequiredLevel();
	SetRequiredLevelColor();
	SetCurrentQueuedAmount();
	SetMaxQueuedAmount();
}

void UCraftingBase::SetCurrentCraftingData()
{
	auto Data = PlayerCrafting->GetCurrentCraftableData();
	CurrentCraftableItem = Data.Get<0>();
	CurrentRequireMaterials = Data.Get<1>();
}

void UCraftingBase::SetItemName() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		TextItemRarity->SetText(CurrentCraftableItem->Class.GetDefaultObject()->NameItem);
	}
}

void UCraftingBase::SetItemType() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		TextItemType->SetText(UEnum::GetDisplayValueAsText(CurrentCraftableItem->Class.GetDefaultObject()->Type));
	}
}

void UCraftingBase::SetItemRarity() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		TextItemRarity->SetText(UEnum::GetDisplayValueAsText(CurrentCraftableItem->Class.GetDefaultObject()->Rarity));
	}
}

void UCraftingBase::SetItemImage() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		ImageItem->SetBrushFromTexture(CurrentCraftableItem->Class.GetDefaultObject()->ImageItem);
	}
}

void UCraftingBase::SetItemDescription() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		auto Description = CurrentCraftableItem->Class.GetDefaultObject()->Description;
		if (UKismetTextLibrary::TextIsEmpty(Description))
			TextDescription->SetVisibility(ESlateVisibility::Collapsed);
		else
		{
			TextDescription->SetVisibility(ESlateVisibility::Visible);
			TextDescription->SetText(Description);
		}
	}
}

void UCraftingBase::SetItemRarityColor() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		TextItemRarity->SetColorAndOpacity(
			InventoryUtility::GetRarityColor(CurrentCraftableItem->Class.GetDefaultObject()->Rarity));
	}
}

void UCraftingBase::BuildStatsSlots()
{
	VerticalBoxItemsStats->ClearChildren();
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		for (auto ItemStat : CurrentCraftableItem->Class.GetDefaultObject()->Stats)
		{
			UItemStatsSlot* StatsSlot = CreateWidget<UItemStatsSlot>(this, ItemStatsSlotClass);
			StatsSlot->InitializeSlot(UEnum::GetDisplayValueAsText(ItemStat.Type), ItemStat.Value,
			                          GetComparisonValue(CurrentCraftableItem->EquipmentSlot, ItemStat), true);

			auto VerticalBoxElement = VerticalBoxItemsStats->AddChildToVerticalBox(StatsSlot);
			VerticalBoxElement->SetHorizontalAlignment(HAlign_Fill);
		}
	}
}

void UCraftingBase::SetItemWeight() const
{
	WeightView->SetValue(CurrentCraftableItem->Class.GetDefaultObject()->Weight);
}

void UCraftingBase::SetItemValue() const
{
	ValueView->SetValue(CurrentCraftableItem->Class.GetDefaultObject()->Value);
}

float UCraftingBase::GetComparisonValue(EItemSlot SlotType, FItemsStatRow Stat) const
{
	auto DataSlot = PlayerEquipment->GetItemByEquipmentSlot(SlotType);
	if (DataSlot.Get<0>())
	{
		for (auto ItemStat : DataSlot.Get<1>()->Class.GetDefaultObject()->Stats)
		{
			if (ItemStat.Type == Stat.Type)
				return ItemStat.Value;
		}
	}
	return 0;
}

void UCraftingBase::SetCurrentQueuedAmount() const
{
	TextQueueAmount->SetText(FText::FromString(FString::FromInt(CraftingStation->GetCraftingQueueLength())));
}

void UCraftingBase::SetMaxQueuedAmount() const
{
	TextMaxQueueAmount->SetText(FText::FromString(FString::FromInt(CraftingStation->GetMaxQueuedItems())));
}

void UCraftingBase::SetRequiredLevelColor() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem) && IsValid(PlayerLeveling))
	{
		auto IsRequiredLevel = CurrentCraftableItem->Class.GetDefaultObject()->RequiredLevel <= PlayerLeveling->
			GetCurrentLevel();
		ImageRequireLevelBackground->SetColorAndOpacity(IsRequiredLevel
			                                                ? FLinearColor(0, 0.25f, .040356f)
			                                                : FLinearColor(.296875f, 0, .026351f));
	}
}

void UCraftingBase::SetItemRequiredLevel() const
{
	if (InventoryUtility::IsItemClassValid(CurrentCraftableItem))
	{
		TextRequireLevel->SetText(FText::FromString(
			FString::FromInt(CurrentCraftableItem->Class.GetDefaultObject()->RequiredLevel)));
	}
}

UWidget* UCraftingBase::Nav_CraftingBar(EUINavigation NavigationType)
{
	return CraftingBar;
}

UWidget* UCraftingBase::Nav_MaterialsBar(EUINavigation NavigationType)
{
	return CraftingMaterialsBar;
}

UWidget* UCraftingBase::Nav_CraftingQueue(EUINavigation NavigationType)
{
	return CraftingQueue->CanBeFocused() ? CraftingQueue : nullptr;
}

UWidget* UCraftingBase::Nav_ParentWidget(EUINavigation NavigationType)
{
	return GetParent();
}
