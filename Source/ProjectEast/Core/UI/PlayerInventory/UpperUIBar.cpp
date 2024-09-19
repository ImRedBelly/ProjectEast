#include "UpperUIBar.h"

#include "Components/Button.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UUpperUIBar::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonClose->OnClicked.AddDynamic(this, &UUpperUIBar::CloseWidget);
}

void UUpperUIBar::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonClose->OnClicked.RemoveDynamic(this, &UUpperUIBar::CloseWidget);
}

void UUpperUIBar::CloseWidget()
{
	//TODO Remove Cast
	switch (CloseWidgetType)
	{
	case EWidgetType::Inventory:
		InventoryUtility::GetPlayerInventory(GetOwningPlayer())->InputCloseWidget();
		break;
	default:
		Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager()->CloseActiveWidget();
		break;
	}
}
