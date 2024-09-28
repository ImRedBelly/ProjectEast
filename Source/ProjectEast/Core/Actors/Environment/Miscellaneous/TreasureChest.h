#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Actors/Environment/BaseObjects/BaseInteractable.h"
#include "TreasureChest.generated.h"

class UInventoryCore;

UCLASS()
class PROJECTEAST_API ATreasureChest : public ABaseInteractable
{
	GENERATED_BODY()

protected:
	ATreasureChest();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryCore* InventoryCore;

	virtual void RemoveInteraction() override;
	virtual void ClientStartInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;
	virtual void Destroyed() override;


	UFUNCTION(BlueprintImplementableEvent)
	void OnAnimateOpenChest(bool IsOpen);

private:
	UPROPERTY()
	AActor* LocalInteractor;

	void OnDestroyed() const;
	
	bool bIsOpen;
};
