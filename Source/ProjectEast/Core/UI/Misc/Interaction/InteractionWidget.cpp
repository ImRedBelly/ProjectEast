#include "InteractionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectEast/Core/Characters/MainPlayerController.h"
#include "ProjectEast/Core/Utils/InventoryUtility.h"

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CachedPlayerController = Cast<AMainPlayerController>(GetOwningPlayer());
	IconButtonGameModule = &FModuleManager::GetModuleChecked<FIconButtonGameModule>(ProjectEast);
	
	IconButtonGameModule->InputDeviceChanged.AddDynamic(this, &UInteractionWidget::OnGamepadToggled);
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
}

void UInteractionWidget::NativeDestruct()
{
	IconButtonGameModule->InputDeviceChanged.RemoveDynamic(this, &UInteractionWidget::OnGamepadToggled);
	Super::NativeDestruct();
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

void UInteractionWidget::SetAppropriateFillingBackground()
{
	ImageFillBorder->SetBrushFromMaterial(IsUsingGamepad() ? CircularFill : SquareFill);
}

void UInteractionWidget::OnGamepadToggled()
{
	SetAppropriateFillingBackground();
	SetFillDecimalValue(0.05f);
}

bool UInteractionWidget::IsUsingGamepad() const
{
	return IconButtonGameModule->IsUsingGamepad();
}
