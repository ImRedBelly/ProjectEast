#include "ItemDataDragAndDropPanel.h"
#include "Components/Image.h"

void UItemDataDragAndDropPanel::InitializePanel(UTexture2D* Texture, FVector2D Size)
{
	ImageTexture = Texture;
	ImageSize = Size;
}

void UItemDataDragAndDropPanel::NativeConstruct()
{
	Super::NativeConstruct();

	FSlateBrush Brush;
	Brush.SetResourceObject(ImageTexture);
	Brush.ImageSize = FVector2D(ImageSize);

	ImageDraggable->SetBrush(Brush);
}
