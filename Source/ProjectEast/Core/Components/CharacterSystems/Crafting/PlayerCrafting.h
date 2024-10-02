#pragma once

#include "CoreMinimal.h"
#include "CraftingCore.h"
#include "PlayerCrafting.generated.h"


class UPlayerInventory;
class UWidgetManager;
class AMainPlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerCrafting : public UCraftingCore
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewItemSelected, FCraftingData&, CraftingData);

public:
	FOnNewItemSelected OnNewItemSelected;

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	void InitializeCrafting(AMainPlayerController* PlayerController, UPlayerInventory* InPlayerInventory);
	void ClientInitializeCrafting(AMainPlayerController* PlayerController, UPlayerInventory* InPlayerInventory);

	void AssignCraftableData(FCraftingData* CraftingData);

	void OpenCraftingWidget(UCraftingCore* CraftingCore, EWidgetType WidgetType);
	void CloseCraftingWidget();

	UFUNCTION()
	void OnItemUsed(FItemData& ItemData);
	void FocusSelectedItem(FCraftingData* CraftingData);
	void SetCurrentCraftingStation(UCraftingCore* StationComponent);
	void AttachNewItemToPlayerPreview(FCraftingData* CraftingData);

	void SecureCraftingCost(FCraftingData* CraftingData, int32 AmountToCraft, UCraftingCore* Station,
	                        AActor* OwningPlayer);
	void ReturnSecuredMaterials(FCraftingData* CraftingData, AActor* OwningPlayer);
	void ReturnSecuredCraftingCost(FCraftingData* CraftingData, UCraftingCore* Station, AActor* OwningPlayer);

	TTuple<FItemData*, TArray<FSingleDTItem>> GetCurrentCraftableData() const;
	TTuple<bool, FText> TryToInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft,
	                                                   UCraftingCore* Sender, AActor* OwningPlayer);
	TTuple<bool, TArray<FItemData*>> SecureMaterialItems(TArray<FSingleDTItem> Materials, int32 AmountToCraft,
	                                                     AActor* OwningPlayer);
	TTuple<bool, FText> TryToCraftItem(FCraftingData* CraftingData, UCraftingCore* Sender, AActor* OwningPlayer);

	bool CanCraft(UCraftingCore* Station) const;
	bool UnlockCraftingRecipe(FString CraftingID);
	bool IsCraftingRecipeLocked(FCraftingData* CraftingData) const;
	bool CanBeAddedToInventory(TArray<FItemData*> CraftableItems, AActor* OwningPlayer, UCraftingCore* Station);

	FCraftingData* GetSelectedCraftingData() const;
	UCraftingCore* GetCurrentStationComponent() const { return CurrentStationComponent; }
	int32 FindItemQuantity(FItemData* ItemData, APlayerController* Controller);

	void ServerTryToCraftItem(UCraftingCore* Sender, FCraftingData* CraftingData, AActor* OwningPlayer);
	void ServerReturnSecuredMaterials(FCraftingData* CraftingData, UCraftingCore* Sender, AActor* OwningPlayer);
	void ServerInitializeCraftingProcess(FCraftingData* CraftingData, int32 AmountToCraft, UCraftingCore* Sender,
	                                     AActor* OwningPlayer);
	void ClientTryToCraftReturnValue(bool ReturnValue, FText Message, UCraftingCore* Sender);
	void ClientInitializeCraftingProcessReturnValue(bool ReturnValue, FCraftingData* CraftingData, int32 AmountToCraft,
	                                                UCraftingCore* Sender, FText Message) const;
	void ClientRefreshUIData(UCraftingCore* Target) const;
	void ClientUnlockCraftingRecipe(FString CraftingID);

protected:
	UPROPERTY(EditAnywhere)
	bool bIsCraftingWidgetOpen = true;
	
private:
	UPROPERTY()
	UCraftingCore* CurrentStationComponent;
	UPROPERTY()
	AMainPlayerController* PlayerController;
	UPROPERTY()
	UPlayerInventory* PlayerInventory;
	UPROPERTY()
	UWidgetManager* WidgetManager;

	FItemData* PreviewItemData = new FItemData();
	FItemData* StoredPreviewData = new FItemData();
	FCraftingData* SelectedCraftingData = new FCraftingData();

	TArray<FString> UnlockedRecipes;
};
