#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerLeveling.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerLeveling : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGainedExperience, float, ExpGained);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelChanged);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentExpChanged);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, uint8, NewLevel);

public:
	FOnGainedExperience OnGainedExperience;
	FOnLevelChanged OnLevelChanged;
	FOnCurrentExpChanged OnCurrentExpChanged;
	FOnLevelUp OnLevelUp;

	uint32 GetCurrentLevel();
	float GetCurrentExperience();
	float GetLevelMaxExperience();
};
