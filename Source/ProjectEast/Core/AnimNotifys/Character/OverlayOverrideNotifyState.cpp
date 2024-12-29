#include "OverlayOverrideNotifyState.h"
#include "ProjectEast/Core/Characters/BaseCharacter.h"

void UOverlayOverrideNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MeshComp->GetAnimInstance()))
	{
		BaseCharacter->SetOverlayOverrideState(OverlayOverrideState);
	}
}

void UOverlayOverrideNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetOverlayOverrideState(0);
	}
}

FString UOverlayOverrideNotifyState::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Overlay Override State: "));
	Name.Append(FString::FromInt(OverlayOverrideState));
	return Name;
}
