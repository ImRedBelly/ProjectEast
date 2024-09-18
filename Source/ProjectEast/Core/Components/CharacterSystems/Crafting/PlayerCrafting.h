#pragma once

#include "CoreMinimal.h"
#include "CraftingCore.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/UI/Crafting/CraftingListSlot.h"
#include "PlayerCrafting.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerCrafting : public UCraftingCore
{
	GENERATED_BODY()
	
		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewItemSelected, FCraftingData, CraftingData);

public:
	void AssignCraftableData(FCraftingData* CraftingData);
	void OpenCraftingWidget(UCraftingCore* CraftingCore, EWidgetType WidgetType);
	void CloseCraftingWidget();
	void InitializeCrafting(AMainPlayerController* PlayerController);
	void ClientInitializeCrafting(AMainPlayerController* PlayerController);
	bool IsCraftingRecipeLocked(FCraftingData* CraftingData);
	void FocusSelectedItem(FCraftingData* CraftingData);

	TTuple<FItemData*, TArray<FSingleDTItem>> GetCurrentCraftableData();
	void SetCurrentCraftingStation(UCraftingCore* StationComponent);
	UCraftingCore* GetCurrentStationComponent() const { return CurrentStationComponent; }
	int32 FindItemQuantity(FItemData* ItemData, APlayerController* Controller);

	FOnNewItemSelected OnNewItemSelected;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UCraftingCore* CurrentStationComponent;
	UPROPERTY()
	AMainPlayerController* PlayerController;
	UPROPERTY()
	UWidgetManager* WidgetManager;

	FCraftingData* SelectedCraftingData = new FCraftingData();

	TArray<FString> UnlockedRecipes;

	bool bIsCraftingWidgetOpen;
};
