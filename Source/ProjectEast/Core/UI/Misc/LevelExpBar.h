#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "ProjectEast/Core/Components/PlayerLeveling.h"
#include "LevelExpBar.generated.h"

UCLASS()
class PROJECTEAST_API ULevelExpBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextLevel;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextCurrentExp;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextMaxExp;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBarExp;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void UpdateViewParameters();

private:
	UPROPERTY()
	UPlayerLeveling* PlayerLeveling;
};
