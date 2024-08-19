#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CharacterStatsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UCharacterStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterStatsComponent();
	
public:
	void ServerAddStatValue(EStatCategory StatCategory, float Value);
	void ServerRemoveStatValue(EStatCategory StatCategory, float Value);
};
