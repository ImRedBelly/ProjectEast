#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Components/PostProcessComponent.h"
#include "ProjectEast/Core/Components/CharacterSystems/CharacterStatsComponent.h"
#include "ProjectEast/Core/Components/CharacterSystems/ConsumableBuffs.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ItemConsumable.generated.h"

UCLASS()
class PROJECTEAST_API AItemConsumable : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Cooldown = 0.1f;

	void OnUse();
	void OnRemove();

protected:
	virtual void BeginPlay() override;
	
	void PlayAdditionalEffect();
	void RemoveAdditionalEffect();
	void InitializeAnimation();
	void EventAddValueToStat() const;
	void EventRemoveValueFromStat() const;
	void ResetAddValueForDuration();

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeTimeline();
	UFUNCTION(BlueprintCallable)
	void AddStatToConsumableBuffs(float NewValue);
	
	void AddValueForDuration();
	void AddValueOverDuration();
	
	UPROPERTY(EditAnywhere)
	float Value;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 NumberOfReps;



	UPROPERTY(EditAnywhere)
	EStatCategory StatCategory;
	UPROPERTY(EditAnywhere)
	EBuffEffectCategory EffectCategory;
	UPROPERTY(EditAnywhere)
	EAdditionalBuffEffect AdditionalEffect;
	UPROPERTY(EditAnywhere)
	FPostProcessSettings DrunkPostProcess;
	UPROPERTY(EditAnywhere)
	FPostProcessSettings NightVisionPostProcess;
	
	FTimerHandle ForDurationTimer;
	FTimerHandle OverDurationTimer;

private:
	UPROPERTY()
	UTimelineComponent* TimelineEffect;
	UPROPERTY()
	UConsumableBuffs* ConsumableBuffs;
	UPROPERTY()
	UCharacterStatsComponent* StatsComponent;
	UPROPERTY()
	UPostProcessComponent* PostProcessDrunk;
	UPROPERTY()
	UPostProcessComponent* PostProcessNightVision;

	int8 CurrentOfReps;
};
