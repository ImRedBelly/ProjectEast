#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "SwitcherButton.generated.h"

class UWidgetManager;
class UButton;


UCLASS()
class PROJECTEAST_API USwitcherButton : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeButton(UWidgetManager* WidgetManager);
	
protected:
	UPROPERTY(EditAnywhere, Category=Widget, meta=(ToolTip="Выбор между открытием отдельного окна или вкладки"))
	bool bOpenWidget;
	UPROPERTY(EditAnywhere, Category=Widget)
	EWidgetType WidgetType;

	UPROPERTY(EditAnywhere)
	FText NameButton;
	
	UPROPERTY(EditAnywhere, Category=Widget)
	FLinearColor NormalColor;
	UPROPERTY(EditAnywhere, Category=Widget)
	FLinearColor HoveredColor;
	UPROPERTY(EditAnywhere, Category=Widget)
	FLinearColor UnHoveredColor;

	UPROPERTY(meta=(BindWidget))
	UButton* SwitchButton;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextNameButton;

private:
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;

	UFUNCTION()
	void OnClickedButton();
	UFUNCTION()
	void OnHoveredButton();
	UFUNCTION()
	void OnUnHoveredButton();
	UFUNCTION()
	void OnUpdateViewButton(EWidgetType Widget);

	EWidgetType GetActiveTab();

	UPROPERTY()
	UWidgetManager* CachedWidgetManager;
};
