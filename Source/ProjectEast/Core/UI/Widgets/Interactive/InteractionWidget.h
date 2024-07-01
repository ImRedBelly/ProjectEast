#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Components/Interactive/InteractableComponent.h"
#include "InteractionWidget.generated.h"


class UTextBlock;
class UImage;

UCLASS()
class PROJECTEAST_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetText(FString Interact) const;
	void SetInputType(EInteractionInputType InteractionInput);
	void OnBorderFill(float Value);
	void SetFillDecimalValue(float Value) const;

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageFillBorder;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextBlockInteract;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FillAnimOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Materials Fill")
	UMaterial* SquareFill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Materials Fill")
	UMaterial* CircularFill;

	EInteractionInputType InputType;

	void SetAppropriateFillingBackground() const;
	void BindEventDispatchers();
	void UnbindEventDispatchers();

	//FString GetInteractionText();

	UFUNCTION()
	void OnGamepadToggled();
	bool IsUsingGamepad() const;
//
// private:
// 	FString NameInteract;
};
