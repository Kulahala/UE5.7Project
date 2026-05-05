#include "GameFramework/CharacterMovementComponent.h"
#include "Character/BaseCharacter.h"
#include "AttributeComponent/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KismetAnimationLibrary.h"

// ==================== 生命周期 ====================

ABaseCharacter::ABaseCharacter()
{
	// 旋转设置：关闭控制器旋转，启用面向移动方向
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	PrimaryActorTick.bCanEverTick = true;

	// 属性组件
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GroundSpeed = GetVelocity().Size2D();
	Direction = UKismetAnimationLibrary::CalculateDirection(GetVelocity(), GetActorRotation());
}

// ==================== 受击/战斗 ====================

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator)
{
	IHitInterface::GetHit_Implementation(ImpactPoint, HitInstigator);
	if (Attributes->IsAlive()) //存活
	{
		DirectionalHitReact(ImpactPoint, HitInstigator);
	}

	PlayHitEffects(ImpactPoint);
}

void ABaseCharacter::PlayHitEffects(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
	}
}

float ABaseCharacter::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
                                 class AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint, const AActor* HitInstigator)
{
	//敌人的方向向量
	const FVector Forward = GetActorForwardVector().GetSafeNormal2D();

	//受击来源方向
	FVector ToHit;
	if (HitInstigator)
	{
		//把攻击者所在的方向，作为受力判定的来源点
		ToHit = (HitInstigator->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	}
	else
	{
		ToHit = (ImpactPoint - GetActorLocation()).GetSafeNormal2D();
	}

	//方向向量与受击方向向量夹角
	double Theta = GetHitDirection(Forward, ToHit);
	FName SectionName;
	if (Theta >= -45.f && Theta <= 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (Theta > 45.f && Theta <= 135.f)
	{
		SectionName = FName("FromRight");
	}
	else if (Theta < -45.f && Theta >= -135.f)
	{
		SectionName = FName("FromLeft");
	}
	else
	{
		SectionName = FName("FromBack");
	}
	PlayHitReactMontage(SectionName);
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::Equip()
{
}

// ==================== 蒙太奇 ====================

void ABaseCharacter::PlayAttackMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABaseCharacter::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABaseCharacter::OnHitReactMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactMontage);
	}
}

bool ABaseCharacter::CanAttack() const
{
	return false;
}

void ABaseCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

void ABaseCharacter::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}
