#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

class USpringArmComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Initialize(USpringArmComponent* SpringArmComponent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 570.0f;

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();
	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chatacter | Camera")
	UCurveFloat* CameraTargetArmUpdateCurve;

private:

	bool bIsSprinting;

	USpringArmComponent* SpringArmComponent;
	
	FTimeline CameraArmChangeTimeline;
	float TimelineValue;
	float CurveFloatValue;
	
	UFUNCTION()
	void OnCameraArmChange(float Value) const;
};
