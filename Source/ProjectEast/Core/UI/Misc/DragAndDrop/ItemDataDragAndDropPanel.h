#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDataDragAndDropPanel.generated.h"

class UImage;

UCLASS()
class PROJECTEAST_API UItemDataDragAndDropPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeImage(UTexture2D* Texture2D);

	UPROPERTY(meta=(BindWidget))
	UImage* ImageDraggable;

	UPROPERTY(meta=(BindWidget))
	UImage* WrongSlotImage;

	UPROPERTY(meta=(BindWidget))
	UImage* DropSlotImage;

	UPROPERTY(meta=(BindWidget))
	UImage* SwapSlotImage;


	UPROPERTY(EditAnywhere)
	FVector2D ImageSize;

	virtual void NativeConstruct() override;

private:
	UTexture2D* ImageTexture;
};
