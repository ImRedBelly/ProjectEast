﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectEast/Core/Actors/Environment/BaseObjects/BaseInteractable.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/CraftingCore.h"
#include "Forge.generated.h"

class UCraftingWindowCore;

UCLASS()
class PROJECTEAST_API AForge : public ABaseInteractable
{
	GENERATED_BODY()

public:
	AForge();
	virtual void ClientStartInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;

private:
	UPROPERTY(EditAnywhere)
	UCraftingCore* CraftingCore;
};