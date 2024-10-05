#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "PlayerCapture.generated.h"

class UPlayerEquipment;

UCLASS()
class PROJECTEAST_API APlayerCapture : public AActor
{
	GENERATED_BODY()

public:
	APlayerCapture();
	void AddCaptureRotation(float RotationZ) const;
	void InitializePlayerCapture(UPlayerEquipment* PlayerEquipment);
	void StartCapture();
	void EndCapture();

protected:
	UPROPERTY(EditInstanceOnly)
	UTextRenderComponent* WindPosition;

	UPROPERTY(EditInstanceOnly)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditInstanceOnly)
	USkeletalMeshComponent* SkeletalMeshBoots;
	UPROPERTY(EditInstanceOnly)
	USkeletalMeshComponent* SkeletalMeshLegs;
	UPROPERTY(EditInstanceOnly)
	USkeletalMeshComponent* SkeletalMeshGloves;
	UPROPERTY(EditInstanceOnly)
	USkeletalMeshComponent* SkeletalMeshChest;
	UPROPERTY(EditInstanceOnly)
	USkeletalMeshComponent* SkeletalMeshHead;
	UPROPERTY(EditInstanceOnly)
	USceneCaptureComponent2D* SceneCaptureComponent;
	UPROPERTY(EditInstanceOnly)
	UWindDirectionalSourceComponent* WindDirectionalSourceComponent;

	void BindEventDispatchers();
	void UnbindEventDispatchers();

	UFUNCTION()
	void AttachItem(FItemData& ItemData);
	UFUNCTION()
	void DetachItem(FItemData& ItemData);
	void SpawnWindComponent(float Strength, float Speed);

private:
	UPlayerEquipment* CachedPlayerEquipment;
	USceneComponent* RootCharacter;
};
