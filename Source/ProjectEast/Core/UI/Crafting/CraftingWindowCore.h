#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/CharacterSystems/Crafting/CraftingCore.h"
#include "CraftingWindowCore.generated.h"

UCLASS()
class PROJECTEAST_API UCraftingWindowCore : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWindow(UCraftingCore* CraftingCore);
};
