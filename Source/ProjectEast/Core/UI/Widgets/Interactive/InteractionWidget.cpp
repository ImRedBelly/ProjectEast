#include "InteractionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIconInteraction();
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
	InputType = InteractionInput;
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

void UInteractionWidget::SetIconInteraction() const
{
	UTexture2D* Texture = InventoryUtility::GetGamepadIcon(EGamepadButtonType::FaceButtonBottom);
	ImageInteraction->SetBrushFromTexture(Texture);
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

void UInteractionWidget::OnGamepadToggled()
{
	SetIconInteraction();
	SetAppropriateFillingBackground();
	SetFillDecimalValue(0.05f);
}

bool UInteractionWidget::IsUsingGamepad() const
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayer());

	if (IsValid(PlayerController))
		return PlayerController->IsUsingGamepad();
	
	return false;
}
