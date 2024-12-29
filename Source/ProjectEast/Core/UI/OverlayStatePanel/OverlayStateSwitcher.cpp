#include "OverlayStateSwitcher.h"

#include "OverlayStateButton.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "ProjectEast/Core/Characters/BaseCharacter.h"
#include "ProjectEast/Core/Characters/Interfaces/CharacterInfo.h"
#include "ProjectEast/Core/Utils/GameTypes.h"

void UOverlayStateSwitcher::CycleState(bool bIsUp)
{
	TempEnum.Empty();

	for (auto i = 0; i <= static_cast<int32>(EOverlayState::Barrel); ++i)
	{
		EOverlayState OverlayState = static_cast<EOverlayState>(i);
		TempEnum.Add(OverlayState);
	}

	int32 IndexCurrentState = TempEnum.Find(NewOverlayState) + (bIsUp ? 1 : -1);

	if (IndexCurrentState < 0)
		IndexCurrentState = TempEnum.Num() - 1;
	if (IndexCurrentState >= TempEnum.Num())
		IndexCurrentState = 0;

	if (IndexCurrentState >= 0 && IndexCurrentState < TempEnum.Num())
	{
		NewOverlayState = TempEnum[IndexCurrentState];
		UpdateButtonFocus();
	}
}

void UOverlayStateSwitcher::SelectOverlayState()
 {
 	if (!IsValid(BaseCharacter))
 		BaseCharacter = Cast<ABaseCharacter>(GetOwningPlayer()->GetCharacter());
 	if (IsValid(BaseCharacter))
 	{
 		BaseCharacter->SetOverlayState(NewOverlayState);
 	}
 }

void UOverlayStateSwitcher::NativeConstruct()
{
	Super::NativeConstruct();

	if (ICharacterInfo* CharacterInfo = Cast<ICharacterInfo>(GetOwningPlayer()->GetCharacter()))
	{
		NewOverlayState = CharacterInfo->GetCurrentStates().OverlayState;

		CreateButtons();
		UpdateButtonFocus();
	}
}

void UOverlayStateSwitcher::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!IsValid(BaseCharacter))
		BaseCharacter = Cast<ABaseCharacter>(GetOwningPlayer()->GetCharacter());
	if (IsValid(BaseCharacter))
	{
		if (IsValid(MovablePanel))
		{
			FVector2D ScreenLocation;
			FVector TargetLocation = BaseCharacter->GetTPPivotTarget().GetLocation() + FVector(0, 0, 100);
			UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), TargetLocation,
			                                                           ScreenLocation, false);
			UWidgetLayoutLibrary::SlotAsCanvasSlot(MovablePanel)->SetPosition(ScreenLocation);
		}
	}
}

void UOverlayStateSwitcher::CreateButtons()
{
	for (auto i = 0; i <= static_cast<int32>(EOverlayState::Barrel); ++i)
	{
		EOverlayState OverlayState = static_cast<EOverlayState>(i);

		UOverlayStateButton* NewOverlayStateButton = CreateWidget<UOverlayStateButton>(
			GetOwningPlayer(), OverlayStateButtonClass);
		NewOverlayStateButton->SetNameButton(UEnum::GetDisplayValueAsText(OverlayState));

		VerticalBox->AddChildToVerticalBox(NewOverlayStateButton);

		FOverlayStateButtonParams OverlayStateButtonParams;
		OverlayStateButtonParams.Button = NewOverlayStateButton;
		OverlayStateButtonParams.OverlayState = OverlayState;
		OverlayStateButtons.Add(OverlayStateButtonParams);
	}
}

void UOverlayStateSwitcher::UpdateButtonFocus()
{
	for (FOverlayStateButtonParams OverlayStateButton : OverlayStateButtons)
	{
		if (IsValid(OverlayStateButton.Button))
		{
			OverlayStateButton.Button->SetVisualParameters(
				NewOverlayState == OverlayStateButton.OverlayState);
		}
	}
}
