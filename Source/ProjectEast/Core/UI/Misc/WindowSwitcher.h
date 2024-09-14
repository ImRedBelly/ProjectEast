#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WindowSwitcher.generated.h"

class USwitcherButton;
class UWidgetManager;

UCLASS()
class PROJECTEAST_API UWindowSwitcher : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* ButtonCrafting;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* ButtonInventory;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* ButtonAbilities;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* ButtonMap;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* ButtonQuests;


private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY()
	UWidgetManager* WidgetManager;

};
