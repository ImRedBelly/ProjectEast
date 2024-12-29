#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "OverlayStateSwitcher.generated.h"

class ABaseCharacter;
class UOverlayStateButton;


struct FOverlayStateButtonParams
{
	UOverlayStateButton* Button;
	EOverlayState OverlayState;
};


UCLASS()
class PROJECTEAST_API UOverlayStateSwitcher : public UUserWidget
{
	GENERATED_BODY()

public:
	void CycleState(bool bIsUp);
	void SelectOverlayState();

protected:
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* MovablePanel;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayStateButton> OverlayStateButtonClass;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void CreateButtons();
	void UpdateButtonFocus();

private:
	TArray<FOverlayStateButtonParams> OverlayStateButtons;
	TArray<EOverlayState> TempEnum;
	EOverlayState NewOverlayState;

	ABaseCharacter* BaseCharacter;
};
