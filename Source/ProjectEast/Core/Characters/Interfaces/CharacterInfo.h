#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Utils/Structurs.h"
#include "UObject/Interface.h"
#include "CharacterInfo.generated.h"

UINTERFACE(MinimalAPI)
class UCharacterInfo : public UInterface
{
	GENERATED_BODY()
};


class PROJECTEAST_API ICharacterInfo
{
	GENERATED_BODY()

public:

	virtual FCharacterStates GetCurrentStates();
	virtual FEssentialValues GetEssentialValues();
};
