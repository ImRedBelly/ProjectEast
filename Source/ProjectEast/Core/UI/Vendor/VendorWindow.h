#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/PlayerCrafting.h"
#include "ProjectEast/Core/UI/Core/BaseWindow.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "VendorWindow.generated.h"

class UWidgetManager;
class UPlayerInventory;
class UPlayerInventoryWidget;
class UVendorInventory;
class UKeybindingsSwitcher;
class UGoldWeightBar;
class UEquipmentPanel;

UCLASS(Blueprintable, BlueprintType)
class PROJECTEAST_API UVendorWindow : public UBaseWindow
{
	GENERATED_BODY()
public:
	void InitializeWindow(UPlayerInventory* InPlayerInventory, UWidgetManager* InWidgetManager);
protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(meta=(BindWidget))
	UEquipmentPanel* EquipmentPanel;
	UPROPERTY(meta=(BindWidget))
	UGoldWeightBar* GoldWeightBar;
	UPROPERTY(meta=(BindWidget))
	UKeybindingsSwitcher* KeybindingsSwitcher;
	UPROPERTY(meta=(BindWidget))
	UPlayerInventoryWidget* PlayerInventoryWidget;
	UPROPERTY(meta=(BindWidget))
	UVendorInventory* VendorInventory;
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ConstructAnimation;

private:
	UPROPERTY()
	UPlayerInventory* PlayerInventory;
	UPROPERTY()
	UWidgetManager* WidgetManager;
	
	virtual void NativeDestruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void SwitchTab(EWidgetType WidgetType);
};
