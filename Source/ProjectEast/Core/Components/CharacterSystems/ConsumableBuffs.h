#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ConsumableBuffs.generated.h"



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UConsumableBuffs : public UActorComponent
{
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefresh);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsed, EItemSlot, UsedSLot, float, Cooldown);
	GENERATED_BODY()

public:
	FOnRefresh OnRefresh;
	FOnItemUsed OnItemUsed;


	void OnUsePocket(int8 IndexPocket);

protected:
	virtual void BeginPlay() override;
	TTuple<bool, float> TryToUseConsumable(EItemSlot ItemSlot);


private:
	UPROPERTY()
	UPlayerEquipment* CachedPlayerEquipment;
	EItemSlot UsedSlot;
};
