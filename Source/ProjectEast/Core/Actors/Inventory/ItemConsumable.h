#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemConsumable.generated.h"

UCLASS()
class PROJECTEAST_API AItemConsumable : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Cooldown = 0.1f;

	void OnRemove();
protected:
	AItemConsumable();
	virtual void BeginPlay() override;

};
