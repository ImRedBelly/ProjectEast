#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CustomDeveloperSettings.generated.h"


UCLASS(Config=Engine)
class PROJECTEAST_API UCustomDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, Config)
	TSoftObjectPtr<UDataTable> KeyMapPtr;

	virtual FName GetContainerName() const override
	{
		return "Project";
	}

	virtual FName GetCategoryName() const override
	{
		return "Game";
	}

	virtual FName GetSectionName() const override
	{
		return "Button Label Settings";
	}
};
