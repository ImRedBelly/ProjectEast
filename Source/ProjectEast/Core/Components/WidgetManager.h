#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/UI/Menus/Pause.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/UI/HUD/MainWindow.h"
#include "ProjectEast/Core/Actors/Other/PlayerCapture.h"
#include "ProjectEast/Core/UI/PlayerInventory/SplitStackPopup.h"
#include "ProjectEast/Core/UI/PlayerInventory/InventoryWindow.h"
#include "WidgetManager.generated.h"

class UStorageWindow;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UWidgetManager : public UActorComponent
{
	GENERATED_BODY()

public:
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


	EWidgetType ActiveWidget;
	EWidgetPopup ActivePopup;
};
