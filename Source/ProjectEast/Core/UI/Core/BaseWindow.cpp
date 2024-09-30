#include "BaseWindow.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Components/WidgetManager.h"


void UBaseWindow::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonClose->OnClicked.AddDynamic(this, &UBaseWindow::CloseWidget);
}

void UBaseWindow::NativeDestruct()
{
	ButtonClose->OnClicked.RemoveDynamic(this, &UBaseWindow::CloseWidget);
	Super::NativeDestruct();
}

void UBaseWindow::CloseWidget()
{
	Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager()->CloseActiveWidget();
}
