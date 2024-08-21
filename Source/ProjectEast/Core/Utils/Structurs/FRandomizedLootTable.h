#pragma once

#include "FRandomizedLootTable.generated.h"

USTRUCT()
struct FRandomizedLootTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere)
	int32 MinLootItems;

	UPROPERTY(EditAnywhere)
	int32 MaxLootItems;
};

