#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "PopupLootReceived.generated.h"

class UPlayerInventory;
class ULootReceivedSlot;

UCLASS()
class PROJECTEAST_API UPopupLootReceived : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializePopup(UPlayerInventory* InPlayerInventory);
	void RemoveLootedItem(ULootReceivedSlot* RemoveSlot);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULootReceivedSlot> LootReceivedSlotClass;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxSlots;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBoxLoot;

private:
	UPROPERTY()
	UPlayerInventory* PlayerInventory;

	TArray<FItemData*> LootedItems;

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void AddToLooted(FItemData& ItemData);

	void DisplayNewSlots();
};
