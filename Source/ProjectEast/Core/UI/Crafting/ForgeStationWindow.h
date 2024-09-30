#pragma once

#include "CoreMinimal.h"
#include "CraftingWindowCore.h"
#include "Components/Button.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ForgeStationWindow.generated.h"

class UPopupItemAdded;

UCLASS()
class PROJECTEAST_API UForgeStationWindow : public UCraftingWindowCore
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UPopupItemAdded* PopupItemAdded;
	
private:
	virtual void InitializeWindow(UPlayerCrafting* InPlayerCraftingCore, UCraftingCore* InCraftingCore) override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnItemCrafted(FCraftingData& CraftingData, AActor* OwnningPlayer);
	
};
