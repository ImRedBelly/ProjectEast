#include "Pause.h"

#include "ProjectEast/Core/Characters/MainPlayerController.h"

void UPause::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonResume->OnClicked.AddDynamic(this, &UPause::UPause::CloseWidget);
}

void UPause::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonResume->OnClicked.RemoveDynamic(this, &UPause::UPause::CloseWidget);
}

void UPause::CloseWidget()
{
	Cast<AMainPlayerController>(GetOwningPlayer())->GetWidgetManager()->CloseActiveWidget();
}
