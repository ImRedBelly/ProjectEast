#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Actors/Environment/BaseObjects/BaseInteractable.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "LootBag.generated.h"

UCLASS()
class PROJECTEAST_API ALootBag : public ABaseInteractable
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryCore* InventoryCore;
	
	ALootBag();
	
	virtual void Interaction(AActor* Interactor) override;
	virtual void EndInteraction(AActor* Interactor) override;
	virtual void RemoveInteraction() override;
	virtual void ClientStartInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;
	virtual void Destroyed() override;
	
private:
	void OnDestroyed() const;

	UPROPERTY()
	AActor* MainInteractor;
	UPROPERTY()
	AActor* LocalInteractor;
};
