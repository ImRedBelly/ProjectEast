#include "ItemConsumable.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void AItemConsumable::OnUse()
{
	switch (EffectCategory)
	{
	case EBuffEffectCategory::AddInstantly:
		{
			EventAddValueToStat();
			OnRemove();
		}
		break;
	case EBuffEffectCategory::AddOverDuration:
		{
			AddValueOverDuration();
			InitializeAnimation();
		}
		break;
	case EBuffEffectCategory::AddForDuration:
		{
			AddValueForDuration();
			InitializeAnimation();
		}
		break;
	default: ;
	}
}

void AItemConsumable::OnRemove()
{
	switch (EffectCategory)
	{
	case EBuffEffectCategory::None:
	case EBuffEffectCategory::AddInstantly:
		RemoveAdditionalEffect();
		break;
	case EBuffEffectCategory::AddOverDuration:
		break;
	case EBuffEffectCategory::AddForDuration:
		EventRemoveValueFromStat();
		break;
	}
	Destroy();
}

void AItemConsumable::BeginPlay()
{
	Super::BeginPlay();
	auto MainPlayerController = Cast<AMainPlayerController>(GetOwner());
	ConsumableBuffs = MainPlayerController->GetConsumableBuffs();
	StatsComponent = MainPlayerController->GetStatsComponent();
	OnUse();
	PlayAdditionalEffect();
	//TODO PlaySound2D
}

void AItemConsumable::PlayAdditionalEffect()
{
	switch (AdditionalEffect)
	{
	case EAdditionalBuffEffect::Drunk:
		if (!IsValid(PostProcessDrunk))
		{
			PostProcessDrunk = NewObject<UPostProcessComponent>(this);
			PostProcessDrunk->Settings = DrunkPostProcess;
			
			PostProcessDrunk->SetupAttachment(RootComponent);
			PostProcessDrunk->RegisterComponent();
		}
		break;
	case EAdditionalBuffEffect::NightVision:
		if (!IsValid(PostProcessNightVision))
		{
			PostProcessNightVision = NewObject<UPostProcessComponent>(this);
			PostProcessNightVision->Settings = NightVisionPostProcess;
			
			PostProcessNightVision->SetupAttachment(RootComponent);
			PostProcessNightVision->RegisterComponent();
		}
		break;
	default: ;
	}
}

void AItemConsumable::RemoveAdditionalEffect()
{
	switch (AdditionalEffect)
	{
	case EAdditionalBuffEffect::Drunk:
		if (IsValid(PostProcessDrunk))
		{
			PostProcessDrunk->DestroyComponent();
			PostProcessDrunk = nullptr;
		}
		break;

	case EAdditionalBuffEffect::NightVision:
		if (IsValid(PostProcessNightVision))
		{
			PostProcessNightVision->DestroyComponent();
			PostProcessNightVision = nullptr;
		}
		break;
	default: ;
	}
}

void AItemConsumable::InitializeAnimation()
{
	ConsumableBuffs->InitializeCircularAnimation(StatCategory);
	InitializeTimeline();
}

void AItemConsumable::EventAddValueToStat() const
{
	StatsComponent->ServerAddStatValue(StatCategory, Value);
}

void AItemConsumable::EventRemoveValueFromStat() const
{
	StatsComponent->ServerRemoveStatValue(StatCategory, Value);
}

void AItemConsumable::ResetAddValueForDuration()
{
	if (EffectCategory == EBuffEffectCategory::AddForDuration)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(ForDurationTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(ForDurationTimer);
			EventRemoveValueFromStat();
			AddValueForDuration();
		}
	}
}

void AItemConsumable::AddStatToConsumableBuffs(float NewValue)
{
	ConsumableBuffs->AddStat(StatCategory, NewValue);
}

void AItemConsumable::AddValueForDuration()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(ForDurationTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(ForDurationTimer,
		                                       this,
		                                       &AItemConsumable::AddValueForDuration,
		                                       Duration,
		                                       false);
		EventAddValueToStat();
	}
	else
		OnRemove();
}


void AItemConsumable::AddValueOverDuration()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(OverDurationTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(OverDurationTimer,
		                                       this,
		                                       &AItemConsumable::AddValueOverDuration,
		                                       UKismetMathLibrary::SafeDivide(Duration, NumberOfReps),
		                                       true);
	}

	switch (StatCategory)
	{
	case EStatCategory::Health:
	case EStatCategory::Stamina:
		{
			EventAddValueToStat();
			CurrentOfReps++;
			if (NumberOfReps <= CurrentOfReps)
				OnRemove();
		}
		break;
	default: ;
	}
}
