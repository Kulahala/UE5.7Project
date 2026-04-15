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

void AMyCharacter::Equip()
{
	AWeapon* OverlapWeapon = Cast<AWeapon>(OverLapItem);
	if (OverlapWeapon && CharacterState == ECharacterState::ECS_Unequipped)
	{
		OverlapWeapon->Equip(GetMesh(), FName("hand_rSocket"));
		EquippedWeapon = OverlapWeapon;
		CharacterState = ECharacterState::ECS_OneHandEquipped;
		ArmWeaponState = EArmWeaponState::AWS_Arming;
	}
}

void AMyCharacter::Attack()
{
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
		PlayAttackMontage();
	}
}

bool AMyCharacter::CanAttack() const
{
	return ActionState == EActionState::EAS_UnOccupied && CharacterState != ECharacterState::ECS_Unequipped&& ArmWeaponState == EArmWeaponState::AWS_Arming;
}


void AMyCharacter::ArmWeapon()
{
	// 基础检查：确保当前没有在做其他动作
	if (ActionState != EActionState::EAS_UnOccupied || CharacterState == ECharacterState::ECS_Unequipped) return;

	if (ArmWeaponState == EArmWeaponState::AWS_Disarming)
	{
		// 准备拔刀
		ActionState = EActionState::EAS_Arming;
		PlayArmMontage(FName("ArmWeapon"));
	}
	else
	{
		ActionState = EActionState::EAS_Arming;
		PlayArmMontage(FName("DisarmWeapon"));
	}
}

void AMyCharacter::PlayAttackMontage()
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

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AMyCharacter::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
}

void AMyCharacter::PlayArmMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ArmMontage)
	{
		AnimInstance->Montage_Play(ArmMontage);
		AnimInstance->Montage_JumpToSection(SectionName, ArmMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AMyCharacter::OnArmMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, ArmMontage);

	}
}

void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

	ActionState = EActionState::EAS_UnOccupied;
}

void AMyCharacter::OnArmMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ActionState = EActionState::EAS_UnOccupied;
	if (ArmWeaponState == EArmWeaponState::AWS_Arming)
	{
		ArmWeaponState = EArmWeaponState::AWS_Disarming;
	}
	else
	{
		ArmWeaponState = EArmWeaponState::AWS_Arming;
	}
}


