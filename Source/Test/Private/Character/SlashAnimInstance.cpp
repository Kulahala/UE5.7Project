// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SlashAnimInstance.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapon/Weapon.h"
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

	if (FMath::IsNearlyZero(GroundSpeed))
	{
		IdleTime += DeltaSeconds;
	}
	else
	{
		IdleTime = 0;
	}

	if (!CharacterMovement)return;
	const FVector Velocity = CharacterMovement->Velocity;
	GroundSpeed = Velocity.Size2D();

	IsFalling = CharacterMovement->IsFalling();

	ZSpeed = Velocity.Z;
	if (!MyCharacter)return;
	CharacterState = MyCharacter->GetCharacterState();
}

void USlashAnimInstance::AnimNotify_OccupyEnd() const
{
	if (MyCharacter)
	{
		MyCharacter->SetActionState(EActionState::EAS_UnOccupied);
	}
}

void USlashAnimInstance::AnimNotify_ArmEnd()
{
	if (MyCharacter)
	{
		MyCharacter->SetArmWeaponState(EArmWeaponState::AWS_Arming);
		ArmWeaponState = EArmWeaponState::AWS_Arming;
		CurrentWeapon = MyCharacter->GetWeapon();
		if (CurrentWeapon)
		{
			CurrentWeapon->AttachMeshToSocket(MyCharacter->GetMesh(), FName("hand_rSocket"));
		}
	}
}

void USlashAnimInstance::AnimNotify_UnArmEnd()
{
	if (MyCharacter)
	{
		MyCharacter->SetArmWeaponState(EArmWeaponState::AWS_Disarming);
		ArmWeaponState = EArmWeaponState::AWS_Disarming;
		CurrentWeapon = MyCharacter->GetWeapon();
		if (CurrentWeapon)
		{
			CurrentWeapon->AttachMeshToSocket(MyCharacter->GetMesh(), FName("SpineSocket"));
		}
	}
}
