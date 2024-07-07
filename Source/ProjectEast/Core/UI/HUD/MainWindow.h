#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/UI/LootBar/LootBar.h"
#include "MainWindow.generated.h"

class UCanvasPanel;

UCLASS()
class PROJECTEAST_API UMainWindow : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	ULootBar* CachedLootBar;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* InitMousePosition;
	
	void ShowLootBar() const;
	void HideLootBar() const;
	void SetMousePositionOnLootBar() const;
};
