#include "AnimNotify/AnimNotify_SetState.h"

#include "Character/MyCharacter.h"

void UAnimNotify_SetActionState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(MeshComp->GetOwner()))
		{
			MyCharacter->SetActionState(NewActionState);
		}
	}
}

void UAnimNotify_SetArmWeaponState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(MeshComp->GetOwner()))
		{
			MyCharacter->SetArmWeaponState(NewArmState);
		}
	}
}