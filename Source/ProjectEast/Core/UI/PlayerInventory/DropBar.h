#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "DropBar.generated.h"

class UWidgetManager;
class UPlayerInventory;

UCLASS()
class PROJECTEAST_API UDropBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonDrop;
	UPROPERTY(meta=(BindWidget))
	UImage* DropImage;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderObject;

	virtual void NativeConstruct() override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;

private:
	UWidgetManager* CachedWidgetManager;
	UPlayerInventory* CachedPlayerInventory;
	
};
