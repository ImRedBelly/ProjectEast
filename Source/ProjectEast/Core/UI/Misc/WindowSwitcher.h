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
	USwitcherButton* InventoryButtonCrafting;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* InventoryButtonInventory;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* InventoryButtonAbilities;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* InventoryButtonMap;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* InventoryButtonQuests;

	UPROPERTY(meta=(BindWidget))
	USwitcherButton* VendorButtonVendor;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* VendorButtonEquipment;

	UPROPERTY(meta=(BindWidget))
	USwitcherButton* StorageButtonStorage;
	UPROPERTY(meta=(BindWidget))
	USwitcherButton* StorageButtonEquipment;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* InventoryPanel;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* VendorPanel;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* StoragePanel;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* LoadGamePanel;

private:
	UPROPERTY()
	UWidgetManager* WidgetManager;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void SetWidgetButtonStyle() const;
	void InitializeButtons() const;
	UCanvasPanel* GetSwitcherPanel() const;
};
