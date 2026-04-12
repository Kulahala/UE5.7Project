// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SlashAnimInstance.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyCharacter = Cast<AMyCharacter>(TryGetPawnOwner());

	if (MyCharacter)
	{
		CharacterMovement = MyCharacter->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (GroundSpeed == 0)
	{
		IdleTime += DeltaSeconds;
	}
	else
	{
		IdleTime = 0;
	}

	if (CharacterMovement)
	{
		GroundSpeed = CharacterMovement->Velocity.Size2D();

		IsFalling = CharacterMovement->IsFalling();

		ZSpeed = CharacterMovement->Velocity.Z;
		if (MyCharacter)
		{
			CharacterState = MyCharacter->GetCharacterState();
		}
	}
}

void USlashAnimInstance::AnimNotify_AttackEnd() const
{
	if (MyCharacter)
	{
		MyCharacter->SetActionState(EActionState::EAS_UnOccupied);
	}
}
