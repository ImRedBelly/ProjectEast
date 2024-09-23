#pragma once

#include "CoreMinimal.h"
#include "CraftingBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "CraftingQueue.generated.h"


class UCraftingQueueSlot;

UCLASS()
class PROJECTEAST_API UCraftingQueue : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeQueue(UCraftingCore* InCraftingCore);
	void SetWidgetToTransferFocus(UUserWidget* UserWidget);
	void SetFocusedSlotIndex(int32 Index);
	bool CanBeFocused() const;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCraftingQueueSlot> CraftingQueueSlotClass;

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* HorizontalBoxSlots;

private:
	UPROPERTY()
	UCraftingCore* CraftingStation;
	UPROPERTY()
	UUserWidget* WidgetToTransferFocus;

	int32 CurrentlyFocusedSlot;

	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void BindEventDispatchers();
	void UnbindEventDispatchers();

	UFUNCTION()
	void CreateSlot();
	UFUNCTION()
	void ItemAddedToQueue(FCraftingData& CraftingData, int32 QueueIndex);
	UFUNCTION()
	void ItemRemovedFromQueue(FCraftingData& CraftingData, int32 QueueIndex);
	void SetFocusToSlot(int32 Index) const;
};
