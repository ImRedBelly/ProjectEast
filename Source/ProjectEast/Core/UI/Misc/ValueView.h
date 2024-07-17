#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ValueView.generated.h"

UCLASS()
class PROJECTEAST_API UValueView : public UUserWidget
{
	GENERATED_BODY()


public:
	void SetItemData(FItemData* ItemData);
};
