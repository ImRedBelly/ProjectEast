#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ToolTip.generated.h"

UCLASS()
class PROJECTEAST_API UToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeToolTip(FItemData* ItemData);
	
	void ShowComparisonToolTip();
	void HideComparisonToolTip();
	
};
