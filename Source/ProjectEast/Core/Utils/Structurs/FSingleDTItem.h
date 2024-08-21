#pragma once
#include "FSingleDTItem.generated.h"

USTRUCT(BlueprintType)
struct FSingleDTItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle TableAndRow;
	
	UPROPERTY(EditDefaultsOnly)
	int32 Quantity;
};