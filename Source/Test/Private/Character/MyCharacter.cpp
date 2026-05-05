// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MyCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapon/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "HUD/PlayerHUDWidget.h"
#include "AttributeComponent/AttributeComponent.h"

// ==================== 生命周期 ====================

AMyCharacter::AMyCharacter()
{
	// 移动设置
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);

	// 相机组件
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("Player"));

	// 玩家 HUD
	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
			PlayerHUDWidget->BindToAttributes(Attributes);
		}
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActionState == EActionState::EAS_Stunning || ActionState == EActionState::EAC_Dead) return;

	UpdateMovementSpeed();

	// 调试：右上角打印生命值和耐力
	if (GEngine && Attributes)
	{
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Red,
			FString::Printf(TEXT("HP: %.1f / %.1f"), Attributes->GetCurrentHealth(), Attributes->GetMaxHealth()));
		GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Green,
			FString::Printf(TEXT("SP: %.1f / %.1f"), Attributes->GetCurrentStamina(), Attributes->GetMaxStamina()));
	}
}

// ==================== 战斗 ====================

void AMyCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		Attributes->UseStamina(15.f);
		Attributes->PauseStaminaRegen();
		ActionState = EActionState::EAS_Attacking;
		PlayAttackMontage(FName("Attack2"));
	}
}

void AMyCharacter::Jump()
{
	if (CanJump() && Attributes && Attributes->CheckStamina(10.f))
	{
		Attributes->UseStamina(10.f);
		Super::Jump();
	}
}

void AMyCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator)
{
	Super::GetHit_Implementation(ImpactPoint, HitInstigator);
	if (Attributes->IsAlive())
	{
		ActionState = EActionState::EAS_Stunning;
	}
}

void AMyCharacter::Die()
{
	ActionState = EActionState::EAC_Dead;

	// 停止移动
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 关闭碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 播放死亡蒙太奇
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Stop(0.1f);
		AnimInstance->Montage_Play(DeathMontage);
	}
}

float AMyCharacter::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
                               class AController* EventInstigator, AActor* DamageCauser)
{
	Attributes->ReceiveDamage(DamageAmount);
	if (!Attributes->IsAlive())
	{
		Die();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AMyCharacter::CanAttack() const
{
	return ActionState == EActionState::EAS_UnOccupied && WeaponState != EWeaponState::ECS_Unequipped &&
		ArmWeaponState == EArmWeaponState::AWS_Arming && Attributes->CheckStamina(15.f);
}

// ==================== 装备 ====================

void AMyCharacter::Equip()
{
	if (OverLapItem && OverLapItem->Implements<UPickupInterface>() && !OverLapItem->GetOwner() && WeaponState == EWeaponState::ECS_Unequipped)
	{
		IPickupInterface::Execute_OnPickup(OverLapItem, this);

		if (AWeapon* Weapon = Cast<AWeapon>(OverLapItem))
		{
			EquippedWeapon = Weapon;
		}
		WeaponState = EWeaponState::ECS_OneHandEquipped;
		ArmWeaponState = EArmWeaponState::AWS_Arming;
	}
}

void AMyCharacter::ArmWeapon()
{
	if (ActionState != EActionState::EAS_UnOccupied || WeaponState == EWeaponState::ECS_Unequipped)
	{
		return;
	}

	ActionState = EActionState::EAS_Arming;
	bIsArming = true;

	if (ArmWeaponState == EArmWeaponState::AWS_Disarming)
	{
		PlayArmMontage(FName("ArmWeapon"));
	}
	else
	{
		PlayArmMontage(FName("DisarmWeapon"));
	}
}

// ==================== 移动 ====================

void AMyCharacter::Sprint()
{
	bIsSprinting = true;
}

void AMyCharacter::StopSprinting()
{
	bIsSprinting = false;
}

void AMyCharacter::Walk()
{
	bIsWalking = true;
}

void AMyCharacter::StopWalking()
{
	bIsWalking = false;
}

void AMyCharacter::UpdateMovementSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	// 奔跑每帧扣耐力
	if (bIsSprinting && ActionState == EActionState::EAS_UnOccupied && !Velocity.IsNearlyZero())
	{
		FVector Forward = GetActorForwardVector();
		Forward.Z = 0.f;
		float Dot = FVector::DotProduct(Velocity.GetSafeNormal(), Forward.GetSafeNormal());
		if (Dot > 0.2f)
		{
			float DeltaTime = GetWorld()->GetDeltaSeconds();
			Attributes->UseStamina(12.f * DeltaTime);
			Attributes->ResetStaminaRegenCooldown();
		}
	}

	float SpeedMultiplier = (ArmWeaponState == EArmWeaponState::AWS_Arming) ? 0.875f : 1.0f;

	if (!Velocity.IsNearlyZero())
	{
		FVector Forward = GetActorForwardVector();
		Forward.Z = 0.f;
		float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), Forward.GetSafeNormal());

		float BaseSpeed = 300.f;

		if (bIsSprinting && ActionState == EActionState::EAS_UnOccupied && DotProduct > 0.2f)
		{
			BaseSpeed = 450.f;
		}
		else if (bIsWalking && ActionState == EActionState::EAS_UnOccupied)
		{
			BaseSpeed = 150.f;
		}

		// 分段移速缩放：全速(前) -> 80%(侧) -> 65%(后)
		if (DotProduct > 0.2f)
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
		GetCharacterMovement()->MaxWalkSpeed = 300.f * SpeedMultiplier;
	}

	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("Current Max Speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Cyan, DebugMsg);
	}
}

// ==================== 蒙太奇 ====================

void AMyCharacter::PlayAttackMontage(const FName& SectionName)
{
	Super::PlayAttackMontage(SectionName);
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
	Super::OnAttackMontageEnded(Montage, bInterrupted);
	ActionState = EActionState::EAS_UnOccupied;
	Attributes->ResumeStaminaRegen();
}

void AMyCharacter::OnArmMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ActionState = EActionState::EAS_UnOccupied;
	bIsArming = false;

	if (ArmWeaponState == EArmWeaponState::AWS_Arming)
	{
		ArmWeaponState = EArmWeaponState::AWS_Disarming;
	}
	else
	{
		ArmWeaponState = EArmWeaponState::AWS_Arming;
	}
}
