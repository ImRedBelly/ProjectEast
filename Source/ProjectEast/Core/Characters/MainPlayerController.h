#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectEast/Core/UI/HUD/MainWindow.h"
#include "ProjectEast/Core/Actors/Interfaces/IWidgetManager.h"
#include "ProjectEast/Core/Actors/Interfaces/GamepadControls.h"
#include "ProjectEast/Core/Actors/Interfaces/ObjectInteraction.h"
#include "ProjectEast/Core/Actors/Other/PlayerCapture.h"
#include "ProjectEast/Core/UI/PlayerInventory/SplitStackPopup.h"
#include "MainPlayerController.generated.h"

class UPlayerInventory;
class UInputMappingContext;
class UInteractionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamepadToggled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedWidget, EWidgetType, WidgetType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedTab, EWidgetType, WidgetType);

UCLASS()
class PROJECTEAST_API AMainPlayerController : public APlayerController, public IObjectInteraction, public IWidgetManager, public IGamepadControls
{
	GENERATED_BODY()

public:
	AMainPlayerController();

	virtual void BeginPlay() override;
	void InitializeInteraction(UInteractableComponent* InteractableComponent);
	void OnInteraction() const;
	virtual void CloseActiveWidget() override;

	virtual bool IsUsingGamepad() override;
	virtual void OpenNewWidget(EWidgetType WidgetType) override;
	virtual void SetActiveWidget(EWidgetType WidgetType) override;
	virtual EWidgetType GetActiveWidget() override;
	virtual EWidgetType GetActivePopup() override;
	virtual void SwitchWidgetTo(EWidgetType WidgetType) override;
	virtual void StartPlayerCapture() override;
	virtual void StopPlayerCapture() override;
	virtual void CloseActivePopup() override;
	virtual void OpenSplitStackPopup(FItemData* ItemData, FItemData* InSlotData,
	UInventoryCore* Sender, UInventoryCore* Receiver, EInputMethodType InputMethod,
	EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget) override;
	
	virtual void InitializeInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void StartInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void EndInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void RemoveInteractionFromObject(UInteractableComponent* InteractableComponent) override;

	UPlayerInventory* GetPlayerInventory() const;
	UPlayerEquipment* GetPlayerEquipment() const;
	APlayerCapture* GetPlayerCapture() const;
	UMainWindow* GetMainWindow() const;

	FOnGamepadToggled OnGamepadToggled;
	FOnSwitchedWidget OnSwitchedWidget;
	FOnSwitchedTab OnSwitchedTab;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<UMainWindow> DefaultMainWindow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widgets")
	TSubclassOf<USplitStackPopup> DefaultSplitStackPopup;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPlayerInventory* PlayerInventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPlayerEquipment* PlayerEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* GamepadIcons;


	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	void InputActionInventory();
	void InitializeComponents();
	bool IsAnyMainWidgetOpen() const;
	void ClientInitializeInteractionWithObject(UInteractableComponent* InteractableComponent);

private:
	EWidgetType ActiveWidget;
	EWidgetPopup ActivePopup;
	AActor* CachedObject;
	
	UMainWindow* CachedMainWindow;
	USplitStackPopup* CachedSplitStackPopup;
	APlayerCapture* CachedPlayerCapture;
};
