#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/Components/Inventory/PlayerEquipment.h"
#include "PlayerCapture.generated.h"

UCLASS()
class PROJECTEAST_API APlayerCapture : public AActor
{
	GENERATED_BODY()

public:
	APlayerCapture();
	void AddCaptureRotation(float RotationZ) const;
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

	virtual void BeginPlay() override;

	void InitializePlayerCapture(UPlayerEquipment* PlayerEquipment);
	void BindEventDispatchers();
	void UnbindEventDispatchers();

	void AttachItem();
	void DetachItem();
	void SpawnWindComponent(float Strength, float Speed);

private:
	UPlayerEquipment* CachedPlayerEquipment;
	USceneComponent* RootCharacter;
};
