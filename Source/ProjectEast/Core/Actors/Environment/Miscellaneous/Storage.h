#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "ProjectEast/Core/Actors/Environment/BaseObjects/BaseInteractable.h"
#include "Storage.generated.h"

UCLASS()
class PROJECTEAST_API AStorage : public ABaseInteractable
{
	GENERATED_BODY()

protected:
	AStorage();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryCore* InventoryCore;

	virtual void ClientStartInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAnimateOpenChest(bool IsOpen);

	bool bIsOpen;
};
