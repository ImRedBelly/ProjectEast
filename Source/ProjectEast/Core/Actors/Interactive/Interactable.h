#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTEAST_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void Initialize();
	virtual void Interaction(AActor* Interactor);
	virtual void EndInteraction(AActor* Interactor);
	virtual void RemoveInteraction();
	virtual void GameLoad();
	virtual void ClientStartInteraction(AActor* Interactor);
	virtual void ClientEndInteraction(AActor* Interactor);
	virtual void ClientPreInteraction(AActor* Interactor);
	virtual bool CanBeInteractedWith();
};
