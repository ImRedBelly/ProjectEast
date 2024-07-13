#pragma once

#include "CoreMinimal.h"
#include "InventoryCore.h"
#include "Components/ActorComponent.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "ProjectEast/Core/UI/Misc/DragAndDrop/ItemDataDragDropOperation.h"
#include "PlayerInventory.generated.h"


class IInteractable;
class AMainPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeAllItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemLooted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUsed);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UPlayerInventory : public UInventoryCore 
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void InitializeInventory(APlayerController* PlayerController) override;
	void ClientInitializeInventory(APlayerController* PlayerController);

	void ServerTakeItem(FItemData* ItemData, UInventoryCore* Sender, AActor* OwningPlayer);
	void ServerTakeAllItems(UInventoryCore* Sender, AActor* OwningPlayer);
	void ServerDropItemOnTheGround(FItemData* ItemData, EItemDestination Initiator, AActor* OwningPlayer);
	void InputCloseWidget();
	void InputInteraction() const;

	void OpenLootBarWidget();
	void CloseLootBarWidget();

	bool IsRefreshOmClosingWidget() const {return bIsRefreshOmClosingWidget;}
	
	FOnTakeItem OnTakeItem;
	FOnTakeAllItems OnTakeAllItems;
	FOnItemLooted OnItemLooted;
	FOnItemUsed OnItemUsed;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsRefreshOmClosingWidget;
	
private:
	AMainPlayerController* CashedPlayerController;
	
	bool bIsLootBarOpen;
	bool bIsInteractableActorWidgetOpen;

	UInteractableComponent* GetCurrentInteractable() const;

};
