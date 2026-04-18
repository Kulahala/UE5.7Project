// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UNiagaraSystem;

UCLASS()
class TEST_API AEnemy : public ACharacter,public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GitHit(const FVector& ImpactPoint) override;

	//计算受击方向
	double GetHitDirection(FVector Forward, FVector ToHit);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	//播放受击蒙太奇
	void PlayHitReactMontage(const FName& SectionName);

	UPROPERTY(EditAnywhere,Category="Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere,Category="Particle")
	UParticleSystem* HitParticle;

public:

};
