// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MyCharacter.h"

#include "AttributeComponent/AttributeComponent.h"
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

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
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

	// 计算速度倍率：只有在真正手持武器的状态下 (AWS_Arming) 才会减速为原来的 0.875
	float SpeedMultiplier = (ArmWeaponState == EArmWeaponState::AWS_Arming) ? 0.875f : 1.0f;

	// 如果角色正在移动
	if (!Velocity.IsNearlyZero())
	{
		// 获取角色朝向并忽略Z轴
		FVector Forward = GetActorForwardVector();
		Forward.Z = 0.f;

		// 计算移动方向与角色正前方的点乘 (1为正前，0为正左/右，-1为正后)
		float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), Forward.GetSafeNormal());

		if (DotProduct > 0.8f) // 夹角约 0~36 度 (正前方)
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.f * SpeedMultiplier; 
		}
		else if (DotProduct > 0.2f) // 夹角约 36~78 度 (前左 / 前右)
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.f * SpeedMultiplier;
		}
		else if (DotProduct >= -0.2f) // 夹角约 78~101 度 (纯左 / 纯右)
		{
			GetCharacterMovement()->MaxWalkSpeed = 325.f * SpeedMultiplier;
		}
		else if (DotProduct >= -0.8f) // 夹角约 101~143 度 (后左 / 后右)
		{
			GetCharacterMovement()->MaxWalkSpeed = 250.f * SpeedMultiplier;
		}
		else // 夹角约 143~180 度 (纯后退)
		{
			GetCharacterMovement()->MaxWalkSpeed = 250.f * SpeedMultiplier; 
		}
	}
	else
	{
		// 静止时恢复默认速度
		GetCharacterMovement()->MaxWalkSpeed = 400.f * SpeedMultiplier;
	}

	// 打印调试信息：在屏幕左上角实时显示当前最大移动速度
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("Current Max Speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Cyan, DebugMsg);
	}
}

void AMyCharacter::Equip()
{
	AWeapon* OverlapWeapon = Cast<AWeapon>(OverLapItem);
	if (OverlapWeapon && CharacterState == EWeaponState::ECS_Unequipped)
	{
		OverlapWeapon->Equip(GetMesh(), FName("RightHandSocket"),this,this);
		EquippedWeapon = OverlapWeapon;
		CharacterState = EWeaponState::ECS_OneHandEquipped;
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
	return ActionState == EActionState::EAS_UnOccupied && CharacterState != EWeaponState::ECS_Unequipped &&
		ArmWeaponState == EArmWeaponState::AWS_Arming;
}


void AMyCharacter::ArmWeapon()
{
	// 基础检查：确保当前没有在做其他动作
	if (ActionState != EActionState::EAS_UnOccupied || CharacterState == EWeaponState::ECS_Unequipped)
	{
		return;
	}

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
