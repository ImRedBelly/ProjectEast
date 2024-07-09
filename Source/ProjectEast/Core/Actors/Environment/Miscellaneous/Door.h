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
	
	UPROPERTY(EditAnywhere)
	float RotationAngle = 90.0f;
	
	virtual void Interaction(AActor* Interactor) override;
	virtual void GameLoad() override;

	void OnUnlock();

private:
	
	bool bIsDoorOpen = false;
};
