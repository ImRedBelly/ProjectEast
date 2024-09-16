#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "CraftingListSwitchButton.generated.h"


UCLASS()
class PROJECTEAST_API UCraftingListSwitchButton : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedButton, ECraftingCategory, Category);

public:
	FOnClickedButton OnClickedButton;

	void RefreshListButton(ECraftingCategory NewCategory);
	ECraftingCategory GetCraftingCategory() const;

protected:
	UPROPERTY(EditAnywhere)
	ECraftingCategory Category;

	UPROPERTY(EditAnywhere)
	FButtonStyle ButtonStyleOn;
	UPROPERTY(EditAnywhere)
	FButtonStyle ButtonStyleOff;
	UPROPERTY(EditAnywhere)
	bool bIsActive;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonSwitch;
	UPROPERTY(meta=(BindWidget))
	UBorder* BorderButton;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimationBorder;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnClicked();

};
