#include "CharacterStatsComponent.h"

UCharacterStatsComponent::UCharacterStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterStatsComponent::ServerAddStatValue(EStatCategory StatCategory, float Value)
{
}

void UCharacterStatsComponent::ServerRemoveStatValue(EStatCategory StatCategory, float Value)
{
}

