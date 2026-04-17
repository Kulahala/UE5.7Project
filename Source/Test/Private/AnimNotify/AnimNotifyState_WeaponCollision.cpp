// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_WeaponCollision.h"
#include "Character/MyCharacter.h"
#include "Items/Weapon/Weapon.h"

void UAnimNotifyState_WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(MeshComp->GetOwner()))
		{
			if (AWeapon* Weapon = MyCharacter->GetWeapon())
			{
				Weapon->SetCollision(ECollisionEnabled::QueryOnly);
			}
		}
	}
}

void UAnimNotifyState_WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(MeshComp->GetOwner()))
		{
			if (AWeapon* Weapon = MyCharacter->GetWeapon())
			{
				Weapon->SetCollision(ECollisionEnabled::NoCollision);
			}
		}
	}
}

