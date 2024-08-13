#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConsumableBuffs.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UConsumableBuffs : public UActorComponent
{
	GENERATED_BODY()

public:
	UConsumableBuffs();

};
