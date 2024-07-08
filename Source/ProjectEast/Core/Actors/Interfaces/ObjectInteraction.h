#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObjectInteraction.generated.h"


class UInteractableComponent;

UINTERFACE(MinimalAPI)
class UObjectInteraction : public UInterface
{
	GENERATED_BODY()
};


class PROJECTEAST_API IObjectInteraction
{
	GENERATED_BODY()

public:
	virtual void InitializeInteractionWithObject(UInteractableComponent* InteractableComponent);
	virtual void StartInteractionWithObject(UInteractableComponent* InteractableComponent);
	virtual void EndInteractionWithObject(UInteractableComponent* InteractableComponent);
	virtual void RemoveInteractionFromObject(UInteractableComponent* InteractableComponent);
};
