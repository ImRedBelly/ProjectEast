#pragma once

#include "CoreMinimal.h"
#include "CraftingWindowCore.h"
#include "Components/Button.h"
#include "ForgeStationWindow.generated.h"

UCLASS()
class PROJECTEAST_API UForgeStationWindow : public UCraftingWindowCore
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonClose;

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void CloseWidget();
};
