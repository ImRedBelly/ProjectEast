#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Actors/Interfaces/ObjectInteraction.h"
#include "InteractionComponent.generated.h"

class UInteractableComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UInteractionComponent : public UActorComponent, public IObjectInteraction
{
	GENERATED_BODY()

public:
	UFUNCTION(Unreliable,Client)
	void InitializeInteraction(APlayerController* PlayerController);
	
	UInteractableComponent* GetInteractableComponent() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace Settings")
	float InteractionTimerRate = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace Settings")
	float CameraTraceLength = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace Settings")
	float OwnerTraceLength = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Debug")
	bool bIsShowDrawTrace = false;

private:
	UInteractableComponent* CurrentInteractableComponent;
	APlayerController* CachedPlayerController;
	APawn* CachedPawn;

	FTimerHandle InteractionTimer;

	void UpdateInteraction();
	void UpdateCurrentInteractable();
	void RemoveInteractionFromCurrent();
	void AssignInteractionToLocal(UInteractableComponent* InteractableComponent);
	bool UseOwnerFPTrace();
	void CheckCurrentInteractableComponent();
};
