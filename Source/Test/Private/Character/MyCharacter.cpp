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

	// 获取当前速度并忽略Z轴（垂直掉落）
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	// 如果角色正在移动
	if (!Velocity.IsNearlyZero())
	{
		// 获取角色朝向并忽略Z轴
		FVector Forward = GetActorForwardVector();
		Forward.Z = 0.f;
		
		// 计算移动方向与角色正前方的点乘 (1为正前，0为正左/右，-1为正后)
		float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), Forward.GetSafeNormal());
		
		if (DotProduct < -0.3f) // 夹角大于约107度 (后退或侧后方)
		{
			GetCharacterMovement()->MaxWalkSpeed = 250.f; // 后退速度
		}
		else if (DotProduct <= 0.3f) // 夹角在72度到107度之间 (纯侧向左或右)
		{
			GetCharacterMovement()->MaxWalkSpeed = 280.f; // 侧向速度
		}
		else // 夹角小于72度 (正前方或侧前方)
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.f; // 向前奔跑速度
		}
	}
	else
	{
		// 静止时恢复默认速度
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

void AMyCharacter::Equip()
{
	AWeapon* OverlapWeapon = Cast<AWeapon>(OverLapItem);
	if (OverlapWeapon && CharacterState == ECharacterState::ECS_Unequipped)
	{
		OverlapWeapon->Equip(GetMesh(), FName("RightHandSocket"));
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
		AnimInstance->Montage_JumpToSection(FName("Attack2"), AttackMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AMyCharacter::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
}

void AMyCharacter::PlayArmMontage(const FName& SectionName)
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


