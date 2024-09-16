#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingWindowCore.generated.h"

class UCraftingCore;
class UPlayerCrafting;

UCLASS()
class PROJECTEAST_API UCraftingWindowCore : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void InitializeWindow(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore);

protected:
	UPROPERTY()
	UPlayerCrafting* PlayerCraftingCore;
	UPROPERTY()
	UCraftingCore* CraftingCore;
};
