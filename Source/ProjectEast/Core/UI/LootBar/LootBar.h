#pragma once

#include "CoreMinimal.h"
#include "LootBarSlot.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Inventory/InventoryCore.h"
#include "LootBar.generated.h"

class UPlayerInventory;
class UButton;
class UVerticalBox;
class UScrollBox;

UCLASS()
class PROJECTEAST_API ULootBar : public UUserWidget
{
	GENERATED_BODY()

public:
	bool IsKeyboardFocus;
	void SetCurrentlyFocusedSlot(ULootBarSlot* LootBarSlot);
	void ScrollWidget(ULootBarSlot* LootBarSlot) const;

	void ActivateLootBar();
	void DeactivateLootBar();
	
	UFUNCTION()
	void TakeItem();

	UFUNCTION()
	void TakeAllItems();

	UFUNCTION()
	void Close();

protected:

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonTake;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonTakeAll;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonClose;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULootBarSlot> DefaultLootBarSlot;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* ScrollBox;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationConstruct;

	UPlayerInventory* PlayerInventory;
	UInventoryCore* OwnerInventory;

	int32 FocusedIndex;

	virtual void NativeConstruct() override;

	void BindEventDispatchers();
	void UnbindEventDispatchers();
	void PlayAnimationConstruct();
	void CreateLootBar();
	void SetOwnerInventory();
	void SetFocusToSlot(int32 Index);
	bool IsValidateOwnerInventory() const;

	UFUNCTION()
	void RefreshLootBar(EInventoryPanels Panel);
};
