#include "MovementActionNotifyState.h"
#include "ProjectEast/Core/Characters/BaseCharacter.h"

void UMovementActionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		BaseCharacter->SetMovementAction(MovementAction);
	}
}

void UMovementActionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		if(BaseCharacter->GetCurrentStates().MovementAction == MovementAction)
		{
			BaseCharacter->SetMovementAction(EMovementAction::None);
		}
	}
}

FString UMovementActionNotifyState::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Movement Action: "));
	Name.Append(UEnum::GetDisplayValueAsText(MovementAction).ToString());
	return Name;
}
