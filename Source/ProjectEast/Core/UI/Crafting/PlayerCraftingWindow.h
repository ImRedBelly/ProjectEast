#pragma once

#include "CoreMinimal.h"
#include "CraftingWindowCore.h"
#include "PlayerCraftingWindow.generated.h"

class UCraftingList;

UCLASS()
class PROJECTEAST_API UPlayerCraftingWindow : public UCraftingWindowCore
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UCraftingList* CraftingList;
	virtual void InitializeWindow(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore) override;
};
