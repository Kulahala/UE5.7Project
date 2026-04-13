// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Character/SlashAniminstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapon/Weapon.h"

AMyCharacter::AMyCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);

	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 供 Controller 调用的装备逻辑
void AMyCharacter::Equip()
{
	if (AWeapon* OverlapWeapon = dynamic_cast<AWeapon*>(OverLapItem))
	{
		OverlapWeapon->Equip(GetMesh(), FName("hand_rSocket"));
		CharacterState = ECharacterState::ECS_OneHandEquipped;
	}
}

// 供 Controller 调用的攻击逻辑
void AMyCharacter::Attack()
{
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Occupying;
		PlayAttackMontage();
	}
}

bool AMyCharacter::CanAttack() const
{
	return ActionState == EActionState::EAS_UnOccupied && CharacterState != ECharacterState::ECS_Unequipped;
}


void AMyCharacter::ArmWeapon()
{
	if (ActionState == EActionState::EAS_UnOccupied && CharacterState != ECharacterState::ECS_Unequipped && ArmWeaponState != EArmWeaponState::AWS_Arming)
	{
		ActionState = EActionState::EAS_Occupying;
		ArmWeaponState = EArmWeaponState::AWS_Arming;
		PlayArmMontage(FName("ArmWeapon"));
	}
	else if (ActionState == EActionState::EAS_UnOccupied && CharacterState != ECharacterState::ECS_Unequipped && ArmWeaponState == EArmWeaponState::AWS_Arming)
	{
		ActionState = EActionState::EAS_Occupying;
		ArmWeaponState = EArmWeaponState::AWS_Disarming;
		PlayArmMontage(FName("DisarmWeapon"));
	}
}

void AMyCharacter::PlayAttackMontage() const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Section = FMath::RandRange(1, 2);
		FName SectionName = FName();
		switch (Section)
		{
		case 1: SectionName = FName("Attack1");
			break;
		case 2: SectionName = FName("Attack2");
			break;
		default: break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void AMyCharacter::PlayArmMontage(FName SectionName) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ArmMontage)
	{
		AnimInstance->Montage_Play(ArmMontage);
		AnimInstance->Montage_JumpToSection(SectionName, ArmMontage);
	}
}
