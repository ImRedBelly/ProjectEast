#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingMaterialsBar.generated.h"


class UCraftingMaterialsSlot;
class UCraftingCore;
class UPlayerCrafting;

UCLASS()
class PROJECTEAST_API UCraftingMaterialsBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWidget(UPlayerCrafting* InPlayerCrafting, UCraftingCore* InCraftingStation);
	void SetCurrentFocusedSlot(int32 INT32);

protected:
	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* UniformGridMaterials;

	UPROPERTY(EditDefaultsOnly)
	int32 RowLength;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCraftingMaterialsSlot> CraftingMaterialsSlotClass;

private:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void RefreshCraftingData();
	UFUNCTION()
	void OnNewItemSelected(FCraftingData& CraftingData);

	void SetCurrentCraftingData();
	void BindEventDispatchers();
	void UnbindEventDispatchers();
	void BuildMaterialsSlots();
	void BuildSampleMaterialsSlots();
	void SetFocusToSlot(int32 Index);

	UPROPERTY()
	UPlayerCrafting* PlayerCrafting;
	UPROPERTY()
	UCraftingCore* CraftingStation;


	FItemData* CurrentCraftableItem;
	TArray<FSingleDTItem> RequiredMaterials;
	int32 CurrentlyFocusedSlot;
};
