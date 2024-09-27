#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Actors/Interfaces/Interactable.h"
#include "ProjectEast/Core/InputDetection/FIconButtonGameModule.h"
#include "InteractableComponent.generated.h"

class UInventoryCore;
class UBoxComponent;
class UWidgetComponent;
class UInteractionWidget;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UInteractableComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:
	UBoxComponent* GetInteractionVolume() const;
	bool IsInteractable() const;
	bool IsShowLootBar() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction Settings")
	TMap<AActor*, int32> AssociatedInteractableActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction Settings")
	bool bIsInteractable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	bool bDestroyAfterPickup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	FString DefaultInteractionText = "Interact";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	float MaxKeyTimeDown = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	float MashingKeyRetriggerableTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	EInteractionInputType InteractionInputType = EInteractionInputType::Single;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	EInteractionResponse InteractableResponse = EInteractionResponse::Persistent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	int InteractableValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	int InteractableTargetValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	int InteractableLimitValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	bool bIsCheckForAssociatedActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	bool bCanReInitialized = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings")
	bool bIsRemoveAssociatedInteractablesOnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction Settings")
	bool bIsShowLootBar = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Settings|SpecifiedPanel")
	bool bUseSpecifiedPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Settings|SpecifiedPanel",meta=(EditCondition="bUseSpecifiedPanel", EditConditionHides))
	FString KeyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Settings|SpecifiedPanel",meta=(EditCondition="bUseSpecifiedPanel", EditConditionHides))
	bool bRemoveItemAfterInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Settings|SpecifiedPanel",meta=(EditCondition="bUseSpecifiedPanel", EditConditionHides))
	EInventoryPanels SpecifiedPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings|Advanced")
	FName InteractableTag = "Interactable";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings|Advanced")
	FName DestroyableTag = "Destroyable";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings|Advanced")
	TSubclassOf<UInteractionWidget> BaseInteractionWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction Settings|Advanced")
	float InteractionTimerRate = 0.05f;

	virtual void BeginPlay() override;

private:
	AActor* CachedInteractor;
	bool bIsAlreadyInteracted;
	UInteractionWidget* CachedInteractionWidget;

	UBoxComponent* InteractableArea;
	UWidgetComponent* InteractionWidgetComponent;
	FIconButtonGameModule* IconButtonGameModule;

	TArray<UPrimitiveComponent*> ObjectsHighlight;


	FTimerHandle KeyDownTimer;
	FEnhancedActionKeyMapping PressedInteractionKey;


	void SetWidgetLocalOwner(APlayerController* OwningPlayer);

public:
	virtual void Initialize() override;
	virtual void EndInteraction(AActor* Interactor) override;
	virtual void RemoveInteraction() override;
	virtual void GameLoad() override;
	virtual void ClientStartInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;
	virtual void ClientPreInteraction(AActor* Interactor) override;
	virtual bool CanBeInteractedWith() override;


#pragma region BeforeInteraction

	void OnInitialize() const;
	void SetupInteractableReferences(UBoxComponent* BoxComponent, UWidgetComponent* WidgetComponent,
	                                 const TArray<UPrimitiveComponent*>& HighlightableObjects);

	void OnToggleHighlight(bool bIsHighlight, AActor* Interactor);
	void OnPreInteraction(AActor* Interactor, TArray<FEnhancedActionKeyMapping> InteractionKeys);
	void OnDurationPress();

	void IsKeyDown();
	void FindPressedKeyByActionName(TArray<FEnhancedActionKeyMapping> InteractionKeys);
	void FillInteractionWidgetBorder(float Value) const;
	TTuple<bool, bool> HoldingInput() const;
	void StartInteraction();

	void OnMultiplePress();
	void ResetClickCounter();

	bool CheckForSpecificItemInInventory(AActor* Interactor) const;

#pragma endregion BeforeInteraction

#pragma region OnInteraction
	virtual void Interaction(AActor* Interactor) override;
	virtual void OnClientInteraction(AActor* Interactor);

	void OnAssociatedActorInteraction(AActor* Interactor);
	void OnCheckForInteractionWithAssociate(AActor* Interactor);
	void RemoveInteractionByResponse();
#pragma endregion OnInteraction

#pragma region AfterInteraction

	void OnEndInteraction(AActor* Interactor) const;
	void OnClientEndInteraction(AActor* Interactor) const;
	void OnAssociatedActorEndInteraction() const;

	void OnRemoveInteraction();
	void OnClientRemoveInteraction();

	void OnReInitialize();
	void OnReInitializeAssociatedActors() const;

	void OnGameLoaded() const;

#pragma endregion AfterInteraction

	void ChangeInteractableValue(bool Increment);
	int GetInteractableValue() const;
	void ToggleInteractionWidget(bool Condition) const;
	bool IsTargetInteractableValue();
	void ToggleCanBeReInitialized(bool Condition);
	void ToggleIsInteractable(bool Condition);


	int32 CurrentClickCount;
	int32 MaxClickCount = 15;
	float SpeedFillBorder = 0.075f;
	float FirstDelayResetClick = 0.5f;
	FTimerHandle ClickResetTimerHandle;
};
