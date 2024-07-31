#pragma once

#include "CoreMinimal.h"
#include "SortSlot.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "SortWindow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSorting);

UCLASS()
class PROJECTEAST_API USortWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnEndSorting OnEndSorting;
	void ToggleSortingWindow() const;

protected:
	UPROPERTY(meta=(BindWidget))
	USortSlot* ButtonSortType;

	UPROPERTY(meta=(BindWidget))
	USortSlot* ButtonSortRarity;

	UPROPERTY(meta=(BindWidget))
	USortSlot* ButtonSortValue;

	UPROPERTY(meta=(BindWidget))
	USortSlot* ButtonSortWeight;


	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void OnSort(ESortMethod SortMethod);

	bool IsUsingGamepad() const;

	AMainPlayerController* CachedPlayerController;
	UPlayerInventory* CachedPlayerInventory;
};
