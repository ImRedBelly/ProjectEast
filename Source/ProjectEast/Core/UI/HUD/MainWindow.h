#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "ProjectEast/Core/UI/LootBar/LootBar.h"
#include "MainWindow.generated.h"

class UPopupGreaterLoot;
class UPopupLootReceived;
class AMainPlayerController;

UCLASS()
class PROJECTEAST_API UMainWindow : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	ULootBar* CachedLootBar;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* InitMousePosition;
	UPROPERTY(meta=(BindWidget))
	UPopupLootReceived* PopupLootReceived;
	UPROPERTY(meta=(BindWidget))
	UPopupGreaterLoot* PopupGreaterLoot;

	virtual void NativeConstruct() override;
	void ShowLootBar() const;
	void HideLootBar() const;
	void SetMousePositionOnLootBar() const;

private:
	UPROPERTY()
	AMainPlayerController* CachedPlayerController;
	FIconButtonGameModule* IconButtonGameModule;
};
