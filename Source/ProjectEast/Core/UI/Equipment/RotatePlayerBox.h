#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "ProjectEast/Core/Actors/Other/PlayerCapture.h"
#include "RotatePlayerBox.generated.h"

UCLASS()
class PROJECTEAST_API URotatePlayerBox : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ImageBackground;
	UPROPERTY(EditAnywhere)
	float SpeedRotation =  0.35f;
	
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	APlayerCapture* PlayerCapture;
};
