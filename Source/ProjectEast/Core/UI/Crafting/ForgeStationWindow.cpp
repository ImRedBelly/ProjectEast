#include "ForgeStationWindow.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"

void UForgeStationWindow::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonClose->OnClicked.AddDynamic(this, &UForgeStationWindow::CloseWidget);
}

void UForgeStationWindow::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonClose->OnClicked.RemoveDynamic(this, &UForgeStationWindow::CloseWidget);
}

void UForgeStationWindow::CloseWidget()
{
	//TODO Remove Cast
	Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager()->CloseActiveWidget();
}
