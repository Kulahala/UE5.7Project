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
	Tags.Add(FName("Player"));
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 获取当前速度并忽略Z轴（垂直掉落）
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	// 持刀状态减速，模拟机动性受限
	float SpeedMultiplier = (ArmWeaponState == EArmWeaponState::AWS_Arming) ? 0.875f : 1.0f;

	// 动态移速：根据移动方向与正前方的夹角（点乘）缩放 MaxWalkSpeed
	if (!Velocity.IsNearlyZero())
	{
		FVector Forward = GetActorForwardVector();
		Forward.Z = 0.f;

		float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), Forward.GetSafeNormal());

		// 基础速度基准
		float BaseSpeed = 300.f; 
		
		// 状态提速：冲刺(Shift) vs 步行(Alt)
		if (bIsSprinting && ActionState == EActionState::EAS_UnOccupied && DotProduct > 0.2f)
		{
			BaseSpeed = 450.f; 
		}
		else if (bIsWalking && ActionState == EActionState::EAS_UnOccupied)
		{
			BaseSpeed = 150.f; 
		}

		// 分段移速缩放：全速(前) -> 80%(侧) -> 65%(后)
		if (DotProduct > 0.8f) 
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SpeedMultiplier; 
		}
		else if (DotProduct > 0.2f) 
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;
		}
		else if (DotProduct >= -0.2f) 
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * 0.8f * SpeedMultiplier;
		}
		else 
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * 0.65f * SpeedMultiplier;
		}
	}
	else
	{
		// 静止时恢复默认速度
		GetCharacterMovement()->MaxWalkSpeed = 300.f * SpeedMultiplier;
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
	if (OverLapItem && OverLapItem->Implements<UPickupInterface>() && CharacterState == EWeaponState::ECS_Unequipped)
	{
		IPickupInterface::Execute_OnPickup(OverLapItem, this);

		if (AWeapon* Weapon = Cast<AWeapon>(OverLapItem))
		{
			EquippedWeapon = Weapon;
		}
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

void AMyCharacter::Sprint()
{
	if (ActionState == EActionState::EAS_UnOccupied)
	{
		bIsSprinting = true;
	}
}

void AMyCharacter::StopSprinting()
{
	bIsSprinting = false;
}

void AMyCharacter::Walk()
{
	if (ActionState == EActionState::EAS_UnOccupied)
	{
		bIsWalking = true;
	}
}

void AMyCharacter::StopWalking()
{
	bIsWalking = false;
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
