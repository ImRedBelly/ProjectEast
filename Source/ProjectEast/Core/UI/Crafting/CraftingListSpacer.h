#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "CraftingListSpacer.generated.h"

class FIconButtonGameModule;

UCLASS()
class PROJECTEAST_API UCraftingListSpacer : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetListTitle(const FString& TitleName) const;

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(meta=(BindWidget))
	UImage* ImageTriangle;
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonList;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextTitle;

	UPROPERTY(EditAnywhere)
	UVerticalBox* AssociatedBox;
	UPROPERTY(EditAnywhere)
	FString TitleText;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor ColorActive;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor ColorInactive;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void OnClickButton();
	UFUNCTION()
	void OnHoveredButton();
	UFUNCTION()
	void OnUnhoveredButton();

	void ToggleBoxVisibility();

private:
	FIconButtonGameModule* IconButtonGameModule;

	bool bIsToggleVisibility;
};
