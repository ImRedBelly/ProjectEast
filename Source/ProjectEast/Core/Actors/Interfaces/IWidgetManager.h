#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragDropOperation.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "UObject/Interface.h"
#include "IWidgetManager.generated.h"

UINTERFACE(MinimalAPI)
class UWidgetManager : public UInterface
{
	GENERATED_BODY()
};

class PROJECTEAST_API IWidgetManager
{
	GENERATED_BODY()

public:
	virtual void SetActiveWidget(EWidgetType WidgetType);
	virtual EWidgetType GetActiveWidget();
	virtual void SetActiveTab(EWidgetType WidgetType);
	virtual EWidgetType GetActiveTab();

	virtual void CloseActiveWidget();
	virtual void OpenNewWidget(EWidgetType WidgetType);
	virtual void SwitchWidgetTo(EWidgetType WidgetType);
	virtual void SwitchTabTo(EWidgetType WidgetType);
	virtual void OpenSplitStackPopup(FItemData* ItemData, FItemData* InSlotData,
	                                 UInventoryCore* Sender, UInventoryCore* Receiver,
	                                 EInputMethodType InputMethod,
	                                 EItemDestination Initiator, EItemDestination Destination,
	                                 UUserWidget* SenderWidget);
	virtual void OpenConfirmationPopup(const FString Str, FItemData* ItemData, const FItemData* ItemData1,
	                                   const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory,
	                                   EInputMethodType RightClick,
	                                   EItemDestination InventorySlot, EItemDestination DropBar,
	                                   UUserWidget* UserWidget);
	virtual void OpenTextDocumentPopup(FItemData* ItemData, UUserWidget* ParentWidget);

	virtual void CloseActivePopup();
	virtual EWidgetType GetActivePopup();
	virtual void DisplayMessageNotify(const FString Str);
	virtual EWidgetType GetCurrentPopupType();
	virtual void StartPlayerCapture();
	virtual void StopPlayerCapture();
};
