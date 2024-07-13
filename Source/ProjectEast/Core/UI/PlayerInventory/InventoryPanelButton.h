#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "InventoryPanelButton.generated.h"

class UButton;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedButtonPanel, EInventoryPanels, Panel);


UCLASS()
class PROJECTEAST_API UInventoryPanelButton : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnClickedButtonPanel OnClickedButtonPanel;
	void RefreshPanelButton(EInventoryPanels ActivePanel) const;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderBackground;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInventoryPanels Panel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FButtonStyle ButtonStyleOn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FButtonStyle ButtonStyleOff;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
	UFUNCTION()
	void ButtonOnClicked();
	
};
