#include "Components/Image.h"
#include "ItemDataDragAndDropPanel.h"

void UItemDataDragAndDropPanel::InitializeImage(UTexture2D* Texture)
{
	ImageTexture = Texture;
}

void UItemDataDragAndDropPanel::NativeConstruct()
{
	Super::NativeConstruct();

	FSlateBrush Brush;
	Brush.SetResourceObject(ImageTexture);
	Brush.ImageSize = FVector2D(ImageSize);

	ImageDraggable->SetBrush(Brush);
}
