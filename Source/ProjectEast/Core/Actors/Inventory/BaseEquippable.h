#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "BaseEquippable.generated.h"

UCLASS()
class PROJECTEAST_API ABaseEquippable : public AActor
{
	GENERATED_BODY()
	
public:
	void InitializeItem(EItemSlot ItemSlot);
	void EventAttachToNewSocket(FName SocketName);
	void EventToggleCombatMode(bool bCombatMode);
};
