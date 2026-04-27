// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class AAIController;
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

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;
	double GetHitDirection(const FVector& Forward, const FVector& ToHit);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;  //受击逻辑
	void DirectionalHitReact(const FVector& ImpactPoint, AActor* HitInstigator);
	void Die(); //死亡演出

protected:
	virtual void BeginPlay() override;
	void CheckPatrolTarget(float DeltaTime); //巡逻逻辑
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayHitReactMontage(const FName& SectionName);
	bool BInTargetRange(AActor* Target, double Range)const;
	FTimerHandle HealthBarHideTimer; //血条延迟隐藏机制
	void ShowHealthBar();
	void HideHealthBar();
	void MoveToPatrolTarget();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HealthBarDisplayTime = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Particle")
	UParticleSystem* HitParticle;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UHealthBarComponent* HealthBarWidgetComp;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	// 战斗目标
	UPROPERTY()
	AActor* ChasingTarget;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	double ChasingRadius = 1000.f; //追击范围

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	double AttackingRadius = 200.f;//战斗范围
	
	/*
	 * 寻路与导航
	 */
	 // AI控制器
	FTimerHandle PatrolTimer;
	void PatrolTimerFinished(); //等待计时器回调
	UPROPERTY()
	AActor* SpawnPoint; // 记录动态生成的出生点
	UPROPERTY()
	AAIController* EnemyController;
	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMin = 3.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMax = 5.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolRotationSpeed = 2.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float SingleLookTime = 1.5f;

	FTimerHandle LookTimer;
	void GenerateNewLookRotation();

	FRotator PatrolWaitTargetRotation;

	

public:
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
};
