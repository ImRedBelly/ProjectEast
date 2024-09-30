#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/UI/Core/BaseWindow.h"
#include "CraftingWindowCore.generated.h"

class UCraftingList;
class UCraftingBase;
class UCraftingCore;
class UPlayerCrafting;

UCLASS(Blueprintable, BlueprintType)
class PROJECTEAST_API UCraftingWindowCore : public UBaseWindow
{
	GENERATED_BODY()

public:
	virtual void InitializeWindow(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore);

protected:
	UPROPERTY(meta=(BindWidget))
	UCraftingBase* CraftingBase;

	UPROPERTY(meta=(BindWidget))
	UCraftingList* CraftingList;

	UPROPERTY()
	UPlayerCrafting* PlayerCraftingCore;
	UPROPERTY()
	UCraftingCore* CraftingCore;
};
