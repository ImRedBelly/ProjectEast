#pragma once

#include "CoreMinimal.h"
#include "StorageInventory.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/UI/Equipment/EquipmentPanel.h"
#include "ProjectEast/Core/UI/PlayerInventory/UpperUIBar.h"
#include "ProjectEast/Core/UI/PlayerInventory/GoldWeightBar.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "StorageWindow.generated.h"

UCLASS()
class PROJECTEAST_API UStorageWindow : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcher;
	
	UPROPERTY(meta=(BindWidget))
	UEquipmentPanel* EquipmentPanel;
	UPROPERTY(meta=(BindWidget))
	UGoldWeightBar* GoldWeightPanel;
	// UPROPERTY(meta=(BindWidget))
	// UKeyBindingsSwitcher* KeyBindingsSwitcher;
	UPROPERTY(meta=(BindWidget))
	UPlayerInventoryWidget* PlayerInventoryPanel;
	UPROPERTY(meta=(BindWidget))
	UStorageInventory* StoragePanel;
	UPROPERTY(meta=(BindWidget))
	UUpperUIBar* UpperUIPanel;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationConstruct;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	void BindEventDispatchers();
	void UnbindEventDispatchers();

	UFUNCTION()
	void SwitchTab(EWidgetType WidgetType);

private:
	UPlayerInventory* CachedPlayerInventory;
	
};
