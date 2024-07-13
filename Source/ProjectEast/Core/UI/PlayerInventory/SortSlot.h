#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "SortSlot.generated.h"

class UBorder;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClickedButton, ESortMethod, Method);

UCLASS()
class PROJECTEAST_API USortSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	FClickedButton ClickedButton;

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextTitle;
	
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonSort;
	
	UPROPERTY(meta=(BindWidget))
	UBorder* BackgroundBorder;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESortMethod SortingMethod;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Title = "Title";

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void OnClickedButton();

	UFUNCTION()
	void OnHoveredButton();

	UFUNCTION()
	void OnUnhoveredButton();

private:
	void SetBorderColor(FLinearColor Color) const;
};
