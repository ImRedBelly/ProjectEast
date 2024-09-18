#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingBase.generated.h"

class UCraftingMaterialsBar;

UCLASS()
class PROJECTEAST_API UCraftingBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UCraftingMaterialsBar* CraftingMaterialsBar;

	virtual void NativeConstruct() override;
};
