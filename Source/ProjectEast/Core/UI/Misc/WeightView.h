#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "WeightView.generated.h"

UCLASS()
class PROJECTEAST_API UWeightView : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItemData(FItemData* ItemData);
};
