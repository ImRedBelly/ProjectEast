#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectEast/Core/Actors/Interactive/ObjectInteraction.h"
#include "MainPlayerController.generated.h"

class UInteractionComponent;

   DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamepadToggled);

UCLASS()
class PROJECTEAST_API AMainPlayerController : public APlayerController, public IObjectInteraction
{
	GENERATED_BODY()
	
public:
	AMainPlayerController();

	FOnGamepadToggled OnGamepadToggled;
	bool IsUsingGamepad();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInteractionComponent* InteractionComponent;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
	void InitializeComponents();

	virtual void InitializeInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void StartInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void EndInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void RemoveInteractionFromObject(UInteractableComponent* InteractableComponent) override;
};
