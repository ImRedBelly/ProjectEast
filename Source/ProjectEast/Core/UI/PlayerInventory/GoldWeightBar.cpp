#include "GoldWeightBar.h"

#include "Kismet/KismetTextLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"

void UGoldWeightBar::NativeConstruct()
{
	Super::NativeConstruct();

	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	if (IsValid(PlayerController))
		PlayerInventory = PlayerController->GetPlayerInventory();

	BindEventDispatchers();
	SetOwnerGold();
	SetCurrentWeight();
	SetMaximumWeight();
}

void UGoldWeightBar::NativeDestruct()
{
	UnbindEventDispatchers();
	Super::NativeDestruct();
}

void UGoldWeightBar::BindEventDispatchers()
{
	if (IsValid(PlayerInventory))
	{
		PlayerInventory->OnChangedOwnerGold.AddDynamic(this, &UGoldWeightBar::SetOwnerGold);
		PlayerInventory->OnChangedCurrentWeight.AddDynamic(this, &UGoldWeightBar::SetCurrentWeight);
		PlayerInventory->OnChangedMaximumWeight.AddDynamic(this, &UGoldWeightBar::SetMaximumWeight);
	}
}

void UGoldWeightBar::UnbindEventDispatchers()
{
	if (IsValid(PlayerInventory))
	{
		PlayerInventory->OnChangedOwnerGold.RemoveDynamic(this, &UGoldWeightBar::SetOwnerGold);
		PlayerInventory->OnChangedCurrentWeight.RemoveDynamic(this, &UGoldWeightBar::SetCurrentWeight);
		PlayerInventory->OnChangedMaximumWeight.RemoveDynamic(this, &UGoldWeightBar::SetMaximumWeight);
	}
}

void UGoldWeightBar::SetOwnerGold()
{
	if (IsValid(PlayerInventory))
		TextGold->SetText(UKismetTextLibrary::Conv_DoubleToText(PlayerInventory->GetOwnerGold(), HalfToEven));
}

void UGoldWeightBar::SetCurrentWeight()
{
	if (IsValid(PlayerInventory))
	{
		TextCurrentWeight->SetText(UKismetTextLibrary::Conv_DoubleToText(PlayerInventory->GetCurrentWeight(), HalfToEven));

		TextWeightSpacer->SetColorAndOpacity(PlayerInventory->IsInventoryOverweight() ? RedColor : WhiteColor);
		TextCurrentWeight->SetColorAndOpacity(PlayerInventory->IsInventoryOverweight() ? RedColor : WhiteColor);
		TextMaximumWeight->SetColorAndOpacity(PlayerInventory->IsInventoryOverweight() ? RedColor : WhiteColor);
	}
	
}

void UGoldWeightBar::SetMaximumWeight()
{
	if (IsValid(PlayerInventory))
		TextMaximumWeight->SetText(
			UKismetTextLibrary::Conv_DoubleToText(PlayerInventory->GetMaximumWeight(), HalfToEven));
}
