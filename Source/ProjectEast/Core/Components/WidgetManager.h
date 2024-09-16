#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "WidgetManager.generated.h"

class UInventoryCore;
class UPlayerInventory;
class APlayerCapture;
class UPause;
class UInventoryWindow;
class UCraftingCore;
class UForgeStationWindow;
class UPlayerCraftingWindow;
class UCraftingWindowCore;
class UMainWindow;
class USplitStackPopup;
class AMainPlayerController;
class UStorageWindow;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UWidgetManager : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchWidget, EWidgetType, WidgetType);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchTab, EWidgetType, WidgetType);
	
public:
	FOnSwitchWidget OnSwitchWidget;
	FOnSwitchTab OnSwitchTab;
	
	void InitializeWidgetManager(AMainPlayerController* PlayerController, UPlayerInventory* PlayerInventory);
	void SetActiveWidget(EWidgetType WidgetType);
	EWidgetType GetActiveWidget();

	void SetActiveTab(EWidgetType WidgetType);
	EWidgetType GetActiveTab();

	void OpenNewWidget(EWidgetType WidgetType);
	void SwitchWidgetTo(EWidgetType WidgetType);
	void SwitchTabTo(EWidgetType WidgetType);

	void CloseActiveWidget();
	void CloseActivePopup();

	EWidgetType GetActivePopup();
	EWidgetType GetCurrentPopupType();

	void StartPlayerCapture();
	void StopPlayerCapture();

	EWidgetType GetCurrentlyFocusedWidget();
	void SetCurrentlyFocusedWidget(EWidgetType None);

	APlayerCapture* GetPlayerCapture() const;
	UMainWindow* GetMainWindow() const;

	bool IsAnyMainWidgetOpen() const;

	void OpenSplitStackPopup(FItemData* ItemData, FItemData* InSlotData,
	                         UInventoryCore* Sender, UInventoryCore* Receiver,
	                         EInputMethodType InputMethod,
	                         EItemDestination Initiator, EItemDestination Destination,
	                         UUserWidget* SenderWidget);
	void OpenConfirmationPopup(const FString Str, FItemData* ItemData, const FItemData* ItemData1,
	                           const UInventoryCore* InventoryCore, UInventoryCore* PlayerInventory,
	                           EInputMethodType RightClick,
	                           EItemDestination InventorySlot, EItemDestination DropBar,
	                           UUserWidget* UserWidget);
	void OpenTextDocumentPopup(FItemData* ItemData, UUserWidget* ParentWidget);

	void DisplayMessageNotify(const FString Str);
	void InitializeCraftingWidgets(UCraftingCore* CraftingCore);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UMainWindow> DefaultMainWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UInventoryWindow> DefaultInventoryWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UStorageWindow> DefaultStorageWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UPause> DefaultPauseWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<USplitStackPopup> DefaultSplitStackPopup;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UPlayerCraftingWindow> PlayerCraftingWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UForgeStationWindow> StationCraftingWindow;

private:
	UPROPERTY()
	UMainWindow* CachedMainWindow;
	UPROPERTY()
	UInventoryWindow* CashedInventoryWindow;
	UPROPERTY()
	UStorageWindow* CashedStorageWindow;
	UPROPERTY()
	UPause* CashedPause;
	UPROPERTY()
	APlayerCapture* CachedPlayerCapture;
	UPROPERTY()
	USplitStackPopup* CachedSplitStackPopup;
	UPROPERTY()
	UPlayerInventory* CachedPlayerInventory;
	UPROPERTY()
	AMainPlayerController* CachedPlayerController;
	UPROPERTY()
	UCraftingWindowCore* CachedPlayerCraftingWindow;
	UPROPERTY()
	UCraftingWindowCore* CachedStationCraftingWindow;

	EWidgetType ActiveWidget;
	EWidgetType ActiveTab;
	EWidgetPopup ActivePopup;
};
