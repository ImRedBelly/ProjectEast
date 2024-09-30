#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "WindowSwitcher.generated.h"

class USwitcherButton;
class UWidgetManager;

UCLASS()
class PROJECTEAST_API UWindowSwitcher : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	EWidgetType ActiveWidget;
	UPROPERTY(EditAnywhere)
	EWidgetType ActiveTab;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* SwitcherWidgetPanels;

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

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* MainPanel;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* VendorPanel;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* StoragePanel;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* LoadGamePanel;

private:
	UPROPERTY()
	UWidgetManager* WidgetManager;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetWidgetButtonStyle() const;
	UCanvasPanel* GetSwitcherPanel(EWidgetType Type) const;
};
