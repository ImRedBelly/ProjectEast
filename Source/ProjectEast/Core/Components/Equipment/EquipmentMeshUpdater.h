#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "EquipmentMeshUpdater.generated.h"


class UPlayerEquipment;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UEquipmentMeshUpdater : public UActorComponent
{
	GENERATED_BODY()

public:
	void InitializeMeshUpdater(ACharacter* Character, USkeletalMeshComponent* InSkeletalMeshHead,
	                           USkeletalMeshComponent* InSkeletalMeshGloves,
	                           USkeletalMeshComponent* InSkeletalMeshChest,
	                           USkeletalMeshComponent* InSkeletalMeshLegs,
	                           USkeletalMeshComponent* InSkeletalMeshBoots);

private:
	UPROPERTY()
	UPlayerEquipment* PlayerEquipment;

	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshHead;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshGloves;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshChest;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshLegs;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshBoots;

	virtual void BeginDestroy() override;

	UFUNCTION()
	void UpdateEquipmentMesh(EItemSlot ItemSlot, USkeletalMesh* SkeletalMesh);
};
