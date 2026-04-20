// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "NiagaraFunctionLibrary.h"
#include "components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
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

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	DrawDebugSphere(this->GetWorld(), ImpactPoint, 5, 12, FColor::Red, false, 5.0f, 0, 1.0f);

	const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
	const FVector ToHit = (ImpactPoint - GetActorLocation()).GetSafeNormal2D();

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
		SectionName = "FromLeft";
	}
	else
	{
		SectionName = FName("FromBack");
	}
	PlayHitReactMontage(SectionName);

	if (!HitSound)return;
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	if (!HitParticle)return;
	UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
}
