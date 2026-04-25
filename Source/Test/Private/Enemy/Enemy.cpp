// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "NiagaraFunctionLibrary.h"
#include "AttributeComponent/AttributeComponent.h"
#include "components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/BaseHealthBarWidget.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	HealthBarWidgetComp = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidgetComp->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (Attributes && HealthBarWidgetComp)
	{
		// 绑定广播：当血量改变时，自动调用血条更新
		Attributes->OnHealthChanged.AddDynamic(HealthBarWidgetComp, &UHealthBarComponent::SetHealthPercent);

		// 初始化：设为满血
		HealthBarWidgetComp->SetHealthPercent(Attributes->GetHealthPercent());
	}
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

double AEnemy::GetHitDirection(const FVector Forward, const FVector ToHit)
{
	// forward * tohit = |forward||tohit|* cos(theta)，|forward| = |tohit| = 1
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	//弧度,并转为角度
	double Theta = FMath::RadiansToDegrees(FMath::Acos(CosTheta));
	// 叉乘结果的 Z 轴如果是负数，说明 ImpactPoint 在怪物的左侧
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f; // 将左侧的受击角度变为负数，范围变成 [-180, 180]
	}
	return Theta;
}

float AEnemy::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator,
                         AActor* DamageCauser)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint, AActor* HitInstigator)
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

void AEnemy::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		//强制打断其他动画
		AnimInstance->Montage_Stop(0.1f);

		FName SectionName;
		switch (FMath::RandRange(1, 4))
		{
		case 1:
			SectionName = FName("Section1");
			break;
		case 2:
			SectionName = FName("Section2");
			break;
		case 3:
			SectionName = FName("Section3");
			break;
		case 4:
			SectionName = FName("Section4");
			break;
		default: break;
		}
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator)
{
	DrawDebugSphere(this->GetWorld(), ImpactPoint, 5, 10, FColor::Red, false, 5.0f, 0, 0.5f);

	if (Attributes)
	{
		if (Attributes->IsAlive()) //存活
		{
			DirectionalHitReact(ImpactPoint, HitInstigator);
		}
		else //死亡
		{
			PlayDeathMontage();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
	}
}
