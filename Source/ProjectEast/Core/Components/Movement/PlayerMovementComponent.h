#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

class APlayerCharacter;

UENUM(BlueprintType)
enum class ECustomMovementMode :uint8
{
	CMOVE_None UMETA(DisplayName="None"),
	CMOVE_Sliding UMETA(DisplayName="Sliding"),
	CMOVE_Max UMETA(Hidden),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;
protected:
	APlayerCharacter* GetBaseCharacterOwner() const;

	// virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	// virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

#pragma region Sprint

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 570.0f;

	void StartSprint();
	void StopSprint();
	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }

private:
	bool bIsSprinting;

#pragma endregion Sprint


#pragma region Sliding

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement: Sliding", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlidingSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement: Sliding", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CapsuleRadius = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement: Sliding", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CapsuleHalfHeight = 60.0f;

	void StartSliding();
	void StopSliding();
	FORCEINLINE bool IsSliding() const { return bIsSliding; }

private:
	bool bIsSliding;

#pragma endregion Sliding
};
