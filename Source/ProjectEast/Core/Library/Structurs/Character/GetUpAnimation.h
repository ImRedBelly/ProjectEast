#pragma once
#include "ProjectEast/Core/Utils/GameTypes.h"

#include "GetUpAnimation.generated.h"

USTRUCT(BlueprintType)
struct FGetUpAnimation : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EOverlayState OverlayState;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* GetUpMontage;
};
