// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MyCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapon/Weapon.h"

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
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActionState == EActionState::EAS_Stunning) return;

	UpdateMovementSpeed();
}

void AMyCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
		PlayAttackMontage(FName("Attack2"));
	}
}

void AMyCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator)
{
	Super::GetHit_Implementation(ImpactPoint, HitInstigator);
	ActionState = EActionState::EAS_Stunning;
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

void AMyCharacter::ArmWeapon()
{
	if (ActionState != EActionState::EAS_UnOccupied || CharacterState == EWeaponState::ECS_Unequipped)
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

bool AMyCharacter::CanAttack() const
{
	return ActionState == EActionState::EAS_UnOccupied && CharacterState != EWeaponState::ECS_Unequipped &&
		ArmWeaponState == EArmWeaponState::AWS_Arming;
}

void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnAttackMontageEnded(Montage, bInterrupted);
	ActionState = EActionState::EAS_UnOccupied;
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
