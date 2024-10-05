#include "ConsumableBuffs.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEast/Core/Actors/Inventory/ItemConsumable.h"
#include "ProjectEast/Core/Components/Equipment/PlayerEquipment.h"
#include "ProjectEast/Core/Data/Inventory/MainItemData.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"


void UConsumableBuffs::OnUsePocket(int8 IndexPocket)
{
	EItemSlot Slot = EItemSlot::Pocket1;

	switch (IndexPocket)
	{
	case 0:
		Slot = EItemSlot::Pocket1;
		break;
	case 1:
		Slot = EItemSlot::Pocket2;
		break;
	case 2:
		Slot = EItemSlot::Pocket3;
		break;
	case 3:
		Slot = EItemSlot::Pocket4;
		break;
	default:
		;
	}
	TryToUseConsumable(Slot);
}

void UConsumableBuffs::InitializeCircularAnimation(EStatCategory StatCategory)
{
	if (OnInitializeCircularAnimation.IsBound())
		OnInitializeCircularAnimation.Broadcast(StatCategory);
}

void UConsumableBuffs::AddStat(EStatCategory StatCategory, float NewValue)
{
	StatsDecimal.Add(StatCategory, NewValue);
}

void UConsumableBuffs::BeginPlay()
{
	Super::BeginPlay();
	CachedPlayerEquipment = InventoryUtility::GetPlayerEquipment(GetOwner());
}

TTuple<bool, float> UConsumableBuffs::TryToUseConsumable(EItemSlot ItemSlot)
{
	UsedSlot = ItemSlot;
	auto Data = CachedPlayerEquipment->GetItemByEquipmentSlot(UsedSlot);
	if (Data.Get<0>() && InventoryUtility::IsItemClassValid(Data.Get<1>()))
	{
		auto ConsumableClass = Data.Get<1>()->Class.GetDefaultObject()->ConsumableClass;

		auto ActorClass = Cast<AItemConsumable>(UGameplayStatics::GetActorOfClass(GetWorld(), ConsumableClass));
		if (IsValid(ActorClass))
			ActorClass->OnRemove();

		FActorSpawnParameters Parameters;
		Parameters.Owner = GetOwner();
		Parameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;

		GetWorld()->SpawnActor<AItemConsumable>(ConsumableClass, GetOwner()->GetActorTransform(), Parameters);

		CachedPlayerEquipment->ServerRemoveItemQuantity(Data.Get<1>(), 1);
		if (OnItemUsed.IsBound())
			OnItemUsed.Broadcast(UsedSlot, ConsumableClass.GetDefaultObject()->Cooldown);

		return MakeTuple(true, ConsumableClass.GetDefaultObject()->Cooldown);
	}
	return MakeTuple(false, 0.0f);
}
