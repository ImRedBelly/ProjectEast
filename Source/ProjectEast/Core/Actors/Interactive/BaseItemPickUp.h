#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "GameFramework/Actor.h"
#include "BaseItemPickUp.generated.h"

UCLASS()
class PROJECTEAST_API ABaseItemPickUp : public ABaseInteractable
{
	GENERATED_BODY()

protected:
	AActor* CachedInteractor;
	AActor* CachedLocalInteractor;

	virtual void Interaction(AActor* Interactor) override;
	virtual void EndInteraction(AActor* Interactor) override;
	virtual void ClientStartInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;
	virtual void RemoveInteraction() override;
	virtual void Destroyed() override;

	virtual void GameLoad() override;

	void MulticastSetPhysicsSimulation() const;
	void SetPhysicsSimulationAndCollision() const;

void OnRemoveInteraction();
};
