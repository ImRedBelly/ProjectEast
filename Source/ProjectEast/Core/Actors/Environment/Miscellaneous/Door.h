#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/Actors/Environment/BaseObjects/BaseInteractable.h"
#include "Door.generated.h"

UCLASS()
class PROJECTEAST_API ADoor : public ABaseInteractable
{
	GENERATED_BODY()

protected:
	ADoor();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Interaction(AActor* Interactor) override;
	virtual void GameLoad() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_DoorToggle)
	bool bDoorOpen = false;
	
	UFUNCTION()
	void OnRep_DoorToggle();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAnimateOpenDoor(bool bIsOpen);

	UFUNCTION(BlueprintCallable)
	void OnDoorRotation(FRotator DoorRotation);
};
