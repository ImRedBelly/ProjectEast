#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorLeveling.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UActorLeveling : public UActorComponent
{
	GENERATED_BODY()

public:
	uint32 GetCurrentLevel();
};
