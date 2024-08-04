#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Inventory/PlayerInventory.h"
#include "DropBar.generated.h"

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
	UPlayerInventory* PlayerInventory;
	
};
