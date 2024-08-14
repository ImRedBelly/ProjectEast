#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"
#include "ProjectEast/Core/Actors/Interfaces/GamepadControls.h"
#include "ProjectEast/Core/Actors/Interfaces/ObjectInteraction.h"
#include "ProjectEast/Core/Components/CharacterSystems/ConsumableBuffs.h"
#include "MainPlayerController.generated.h"

class UPlayerInventory;
class UInteractionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamepadToggled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedWidget, EWidgetType, WidgetType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedTab, EWidgetType, WidgetType);

UCLASS()
class PROJECTEAST_API AMainPlayerController : public APlayerController, public IObjectInteraction,
                                              public IGamepadControls
{
	GENERATED_BODY()

public:
	AMainPlayerController();

	void InitializeInteraction(UInteractableComponent* InteractableComponent);
	void OnInteraction() const;

	virtual bool IsUsingGamepad() override;

	virtual void InitializeInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void StartInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void EndInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void RemoveInteractionFromObject(UInteractableComponent* InteractableComponent) override;

	UPlayerInventory* GetPlayerInventory() const;
	UPlayerEquipment* GetPlayerEquipment() const;
	UConsumableBuffs* GetConsumableBuffs() const;
	UWidgetManager* GetWidgetManager() const;

	FOnGamepadToggled OnGamepadToggled;
	FOnSwitchedWidget OnSwitchedWidget;
	FOnSwitchedTab OnSwitchedTab;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPlayerInventory* PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPlayerEquipment* PlayerEquipment;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UConsumableBuffs* ConsumableBuffs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetManager* WidgetManager;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	void InputActionInventory();
	void InitializeComponents();
	void ClientInitializeInteractionWithObject(UInteractableComponent* InteractableComponent);

private:
	UPROPERTY()
	AActor* CachedObject;
};
