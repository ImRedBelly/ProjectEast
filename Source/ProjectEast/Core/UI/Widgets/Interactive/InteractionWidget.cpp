#include "InteractionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetAppropriateFillingBackground();

	ESlateVisibility ImageFillVisibility = ESlateVisibility::Hidden;
	switch (InputType)
	{
	case EInteractionInputType::Single:
		break;
	case EInteractionInputType::Holding:
	case EInteractionInputType::Multiple:
		ImageFillVisibility = ESlateVisibility::Visible;
		break;
	}

	ImageFillBorder->SetVisibility(ImageFillVisibility);
	SetFillDecimalValue(0.05f);
	PlayAnimation(FillAnimOpacity, 0.0f, 0, EUMGSequencePlayMode::PingPong, 1.0f, false);
	BindEventDispatchers();
}

void UInteractionWidget::NativeDestruct()
{
	Super::NativeDestruct();
	UnbindEventDispatchers();
}

void UInteractionWidget::SetText(FString Interact) const
{
	TextBlockInteract->SetText(FText::FromString(Interact));
}

void UInteractionWidget::SetInputType(EInteractionInputType InteractionInput)
{
}

void UInteractionWidget::OnBorderFill(float Value)
{
	StopAnimation(FillAnimOpacity);
	ImageFillBorder->SetVisibility(ESlateVisibility::Visible);
	SetFillDecimalValue(Value);
}

void UInteractionWidget::SetFillDecimalValue(float Value) const
{
	ImageFillBorder->GetDynamicMaterial()->SetScalarParameterValue("Decimal", FMath::Clamp(Value, 0.05f, 1.0f));
}

void UInteractionWidget::SetAppropriateFillingBackground() const
{
	ImageFillBorder->SetBrushFromMaterial(IsUsingGamepad() ? CircularFill : SquareFill);
}

void UInteractionWidget::BindEventDispatchers()
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());

	if (IsValid(PlayerController))
		PlayerController->OnGamepadToggled.AddDynamic(this, &UInteractionWidget::OnGamepadToggled);
}

void UInteractionWidget::UnbindEventDispatchers()
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());

	if (IsValid(PlayerController))
		PlayerController->OnGamepadToggled.RemoveDynamic(this, &UInteractionWidget::OnGamepadToggled);
}
//
// FString UInteractionWidget::GetInteractionText()
// {
// 	return NameInteract;
// }

void UInteractionWidget::OnGamepadToggled()
{
	SetAppropriateFillingBackground();
	SetFillDecimalValue(0.05f);
}

bool UInteractionWidget::IsUsingGamepad() const
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());

	if (IsValid(PlayerController))
		PlayerController->IsUsingGamepad();
	return false;
}
