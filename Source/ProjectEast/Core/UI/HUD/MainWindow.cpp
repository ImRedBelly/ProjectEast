#include "MainWindow.h"
#include "Components/CanvasPanel.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/UI/Misc/Popups/PopupGreaterLoot.h"
#include "ProjectEast/Core/UI/Misc/Popups/LootReceived/PopupLootReceived.h"


void UMainWindow::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);
	PopupLootReceived->InitializePopup(CachedPlayerController->GetPlayerInventory());
	PopupGreaterLoot->InitializePopup(CachedPlayerController->GetPlayerInventory());
}

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
	if (IsValid(CachedPlayerController) || !IconButtonGameModule->IsUsingGamepad())
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
