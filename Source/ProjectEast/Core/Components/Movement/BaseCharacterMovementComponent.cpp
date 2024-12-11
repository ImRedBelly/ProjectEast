#include "BaseCharacterMovementComponent.h"

#include "ProjectEast/Core/Utils/Structurs.h"


void UBaseCharacterMovementComponent::SetMovementSettings(FMovementSettings NewMovementSettings)
{
	CurrentMovementSettings = NewMovementSettings;
	bRequestMovementSettingsChange = true;
}
