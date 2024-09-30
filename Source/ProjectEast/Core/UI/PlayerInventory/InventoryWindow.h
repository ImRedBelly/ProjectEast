#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/UI/Core/BaseWindow.h"
#include "InventoryWindow.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECTEAST_API UInventoryWindow : public UBaseWindow
{
	GENERATED_BODY()

protected:
	virtual void CloseWidget() override;
};
