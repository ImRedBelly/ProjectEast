#include "LevelExpBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void ULevelExpBar::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerLeveling = Cast<AMainPlayerController>(GetOwningPlayer())->GetPlayerLeveling();

	PlayerLeveling->OnCurrentExpChanged.AddDynamic(this, &ULevelExpBar::UpdateViewParameters);
	PlayerLeveling->OnLevelChanged.AddDynamic(this, &ULevelExpBar::UpdateViewParameters);
}

void ULevelExpBar::NativeDestruct()
{
	Super::NativeDestruct();
	if (IsValid(PlayerLeveling))
	{
		PlayerLeveling->OnCurrentExpChanged.RemoveDynamic(this, &ULevelExpBar::UpdateViewParameters);
		PlayerLeveling->OnLevelChanged.RemoveDynamic(this, &ULevelExpBar::UpdateViewParameters);
		PlayerLeveling = nullptr;
	}
}

void ULevelExpBar::UpdateViewParameters()
{
	const auto PlayerLevel = PlayerLeveling->GetCurrentLevel();
	const auto PlayerCurrentExp = PlayerLeveling->GetCurrentExperience();
	const auto PlayerMaxExp = PlayerLeveling->GetLevelMaxExperience();

	const auto ProgressValue = UKismetMathLibrary::NormalizeToRange(PlayerCurrentExp,
	                                                                0, PlayerMaxExp);

	TextLevel->SetText(FText::FromString(FString::FromInt(PlayerLevel)));
	TextCurrentExp->SetText(FText::FromString(FString::FromInt(PlayerCurrentExp)));
	TextMaxExp->SetText(FText::FromString(FString::FromInt(PlayerMaxExp)));
	ProgressBarExp->SetPercent(ProgressValue);
}
