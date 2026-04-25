// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UWidgetComponent;
class UAttributeComponent;
class UNiagaraSystem;

UCLASS()
class TEST_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	//重写受击接口
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;

	//计算受击方向
	double GetHitDirection(FVector Forward, FVector ToHit);

	//重写受伤函数
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)override;

	//播放受击方向动画
	void DirectionalHitReact(const FVector& ImpactPoint, AActor* HitInstigator);
	void PlayDeathMontage();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//播放受击蒙太奇
	void PlayHitReactMontage(const FName& SectionName);

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category="Particle")
	UParticleSystem* HitParticle;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UHealthBarComponent* HealthBarWidgetComp;//角色蓝图组件

public:
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
};
