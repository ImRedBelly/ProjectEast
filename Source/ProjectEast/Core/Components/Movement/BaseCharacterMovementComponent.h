// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectEast/Core/Utils/Structurs.h"
#include "BaseCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	void SetMovementSettings(FMovementSettings NewMovementSettings);

private:
	FMovementSettings CurrentMovementSettings;
	bool bRequestMovementSettingsChange;
};
