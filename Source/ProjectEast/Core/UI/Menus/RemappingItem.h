#pragma once
#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/InputKeySelector.h"
#include "RemappingItem.generated.h"


UCLASS()
class PROJECTEAST_API URemappingItem : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere)
	UInputAction* InputAction;

	UPROPERTY(EditAnywhere)
	UImage* ImageInput;
	UPROPERTY(EditAnywhere)
	FSlateBrush EmptyViewIcon;

	UPROPERTY(meta=(BindWidget))
	UInputKeySelector* InputKey;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextName;


	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void KeySelected(FInputChord Key);

	UFUNCTION()
	void IsSelectingKeyChanged();

private:
	void InitializeItem();
	void OnChangeIcon();
	void SetIconKey(UTexture2D* IconKey) const;

	FKey CurrentKey;
	bool bIsListenForRemap;
};
