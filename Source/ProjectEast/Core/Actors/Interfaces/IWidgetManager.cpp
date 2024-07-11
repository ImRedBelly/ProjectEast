#include "IWidgetManager.h"

EWidgetType IWidgetManager::GetActiveWidget()
{
	return EWidgetType::None;
}

EWidgetType IWidgetManager::GetActiveTab()
{
	return EWidgetType::None;
}

EWidgetType IWidgetManager::GetCurrentPopupType()
{
	return EWidgetType::None;
}

void IWidgetManager::OpenSplitStackPopup(FItemData* ItemData, const FItemData* ItemData1,
	const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory, EInputMethodType RightClick,
	EItemDestination InventorySlot, EItemDestination DropBar, UUserWidget* PlayerInventoryWidget)
{
}

void IWidgetManager::OpenConfirmationPopup(const FString Str, FItemData* ItemData, const FItemData* ItemData1,
	const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory, EInputMethodType RightClick,
	EItemDestination InventorySlot, EItemDestination DropBar, UUserWidget* UserWidget)
{
}

void IWidgetManager::DisplayMessageNotify(const FString Str)
{
}

void IWidgetManager::OpenTextDocumentPopup(FItemData* ItemData, UUserWidget* ParentWidget)
{
}
