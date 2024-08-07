#include "IWidgetManager.h"

void IWidgetManager::SetActiveWidget(EWidgetType WidgetType)
{
}

EWidgetType IWidgetManager::GetActiveWidget()
{
	return EWidgetType::None;
}

void IWidgetManager::SetActiveTab(EWidgetType WidgetType)
{
}

EWidgetType IWidgetManager::GetActiveTab()
{
	return EWidgetType::None;
}

void IWidgetManager::CloseActiveWidget()
{
}

void IWidgetManager::OpenNewWidget(EWidgetType WidgetType)
{
}

void IWidgetManager::SwitchWidgetTo(EWidgetType WidgetType)
{
}

void IWidgetManager::SwitchTabTo(EWidgetType WidgetType)
{
}

EWidgetType IWidgetManager::GetCurrentPopupType()
{
	return EWidgetType::None;
}

void IWidgetManager::StartPlayerCapture()
{
}

void IWidgetManager::StopPlayerCapture()
{
}

void IWidgetManager::OpenSplitStackPopup(FItemData* ItemData, FItemData* InSlotData,
                                         UInventoryCore* Sender, UInventoryCore* Receiver,
                                         EInputMethodType InputMethod,
                                         EItemDestination Initiator, EItemDestination Destination,
                                         UUserWidget* SenderWidget)
{
}

void IWidgetManager::OpenConfirmationPopup(const FString Str, FItemData* ItemData, const FItemData* ItemData1,
                                           const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory,
                                           EInputMethodType RightClick,
                                           EItemDestination InventorySlot, EItemDestination DropBar,
                                           UUserWidget* UserWidget)
{
}

void IWidgetManager::DisplayMessageNotify(const FString Str)
{
}

void IWidgetManager::OpenTextDocumentPopup(FItemData* ItemData, UUserWidget* ParentWidget)
{
}

void IWidgetManager::CloseActivePopup()
{
}

EWidgetType IWidgetManager::GetActivePopup()
{
	return EWidgetType::None;
}
