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
	void SetText(FString Interact) const;
	void SetInputType(EInteractionInputType InteractionInput);
	void OnBorderFill(float Value);
	void SetFillDecimalValue(float Value) const;


protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageInteraction;
	
	UPROPERTY(meta=(BindWidget))
	UImage* ImageFillBorder;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextBlockInteract;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FillAnimOpacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Materials Fill")
	UMaterial* SquareFill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Materials Fill")
	UMaterial* CircularFill;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	EInteractionInputType InputType;

	void SetIconInteraction() const;
	void SetAppropriateFillingBackground() const;
	void BindEventDispatchers();
	void UnbindEventDispatchers();

	UFUNCTION()
	void OnGamepadToggled();

	bool IsUsingGamepad() const;

	//FString GetInteractionText();
	//
	// private:
	// 	FString NameInteract;
};