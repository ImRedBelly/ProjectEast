#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Actors/Environment/BaseObjects/BaseInteractable.h"
#include "Vendor.generated.h"

class UInventoryCore;

UCLASS()
class PROJECTEAST_API AVendor : public ABaseInteractable
{
	GENERATED_BODY()

public:
	AVendor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryCore* InventoryCore;

	virtual void ClientStartInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;

};
