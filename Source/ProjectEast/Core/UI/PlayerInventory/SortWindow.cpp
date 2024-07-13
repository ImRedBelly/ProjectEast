#include "SortWindow.h"
#include "Components/Button.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"

void USortWindow::NativeConstruct()
{
	Super::NativeConstruct();

	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	CachedPlayerInventory = CachedPlayerController->GetPlayerInventory();

	ButtonSortType->ClickedButton.AddDynamic(this, &USortWindow::OnSort);
	ButtonSortRarity->ClickedButton.AddDynamic(this, &USortWindow::OnSort);
	ButtonSortValue->ClickedButton.AddDynamic(this, &USortWindow::OnSort);
	ButtonSortWeight->ClickedButton.AddDynamic(this, &USortWindow::OnSort);
}

void USortWindow::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonSortType->ClickedButton.RemoveDynamic(this, &USortWindow::OnSort);
	ButtonSortRarity->ClickedButton.RemoveDynamic(this, &USortWindow::OnSort);
	ButtonSortValue->ClickedButton.RemoveDynamic(this, &USortWindow::OnSort);
	ButtonSortWeight->ClickedButton.RemoveDynamic(this, &USortWindow::OnSort);
}

FReply USortWindow::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}


void USortWindow::OnSort(ESortMethod SortMethod)
{
	CachedPlayerInventory->ServerSortInventory(CachedPlayerInventory, SortMethod,
	                                           CachedPlayerInventory->GetActivePanel(), false);

	if (OnEndSorting.IsBound())
		OnEndSorting.Broadcast();
}

void USortWindow::ToggleSortingWindow() const
{
	if (IsUsingGamepad())
	{
		if (HasUserFocusedDescendants(GetOwningPlayer()))
		{
			if (OnEndSorting.IsBound())
				OnEndSorting.Broadcast();
		}
		else
		{
			ButtonSortType->SetKeyboardFocus();
		}
	}
}

bool USortWindow::IsUsingGamepad() const
{
	if (IsValid(CachedPlayerController))
		return CachedPlayerController->IsUsingGamepad();
	return false;
}
