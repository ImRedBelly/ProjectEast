#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragDropOperation.h"
#include "ProjectEast/Core/UI/PlayerInventory/PlayerInventoryWidget.h"
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
	EWidgetType GetActiveWidget();
	EWidgetType GetActiveTab();
	EWidgetType GetCurrentPopupType();
	void OpenSplitStackPopup(FItemData* ItemData, const FItemData* ItemData1, const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory, EInputMethodType RightClick, EItemDestination InventorySlot,
	                        EItemDestination DropBar, UUserWidget* PlayerInventoryWidget);
	void OpenConfirmationPopup(const FString Str, FItemData* ItemData, const FItemData* ItemData1, const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory, EInputMethodType RightClick,
	                          EItemDestination InventorySlot, EItemDestination DropBar, UUserWidget* UserWidget);
	void DisplayMessageNotify(const FString Str);
	void OpenTextDocumentPopup(FItemData* ItemData, UUserWidget* ParentWidget);
};
