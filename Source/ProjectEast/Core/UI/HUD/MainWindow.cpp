#include "MainWindow.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void UMainWindow::ShowLootBar() const
{
	CachedLootBar->ActivateLootBar();
	CachedLootBar->SetVisibility(ESlateVisibility::Visible);
	SetMousePositionOnLootBar();
}

void UMainWindow::HideLootBar() const
{
	CachedLootBar->DeactivateLootBar();
	CachedLootBar->SetVisibility(ESlateVisibility::Hidden);
}

void UMainWindow::SetMousePositionOnLootBar() const
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());

	if (IsValid(PlayerController) || !PlayerController->IsUsingGamepad())
	{
		FVector2D pixelPosition;
		FVector2D viewportPosition;

		USlateBlueprintLibrary::LocalToViewport(GetWorld(),
		                                        InitMousePosition->GetCachedGeometry(),
		                                        FVector2D(0, 0),
		                                        pixelPosition,
		                                        viewportPosition);

		int32 X = UKismetMathLibrary::FTrunc(pixelPosition.X);
		int32 Y = UKismetMathLibrary::FTrunc(pixelPosition.Y);

		GetOwningPlayer()->SetMouseLocation(X, Y);
	}
}
