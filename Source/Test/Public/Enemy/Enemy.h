// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Interfaces/HitInterface.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy.generated.h"

class UAIPerceptionComponent;
class AAIController;
class UHealthBarComponent;
class UWidgetComponent;
class UAttributeComponent;
class UNiagaraSystem;

UCLASS()
class TEST_API AEnemy : public ABaseCharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;  //受击逻辑
	void DirectionalHitReact(const FVector& ImpactPoint, const AActor* HitInstigator);
	void Die(); //死亡演出

	UFUNCTION(BlueprintCallable)
	void OnHitReactEnd();//todo 受击动画结束回调

	UFUNCTION(BlueprintCallable)
	void OnAttackEnd();//todo 攻击动画结束回调

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void OnPatrolling(float DeltaTime); //巡逻逻辑
	void CheckCombatTarget(); // 负责持续的状态切换
	void SetEnemyState(EEnemyState NewState); // 状态机：处理进入/退出状态的一次性事件

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayHitReactMontage(const FName& SectionName);
	bool BInTargetRange(AActor* Target, double Range)const;
	FTimerHandle HealthBarHideTimer; //血条延迟隐藏机制
	void ShowHealthBar();
	void HideHealthBar();
	void MoveToTarget(const AActor* Target);
	void ClearPatrolTimers();

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus); // 感知到目标的回调

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
	/*
	 *Components
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UHealthBarComponent* HealthBarWidgetComp;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_UnOccupied;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	// 战斗目标
	UPROPERTY(VisibleInstanceOnly)
	AActor* ChasingTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingRadius = 1000.f; //追击范围

	UPROPERTY(EditAnywhere, Category = "Combat")
	float CombatingRadius = 150.f;//战斗范围
	
	/*
	 * 巡逻
	 */
	FTimerHandle PatrolTimer;
	void PatrolTimerFinished(); //等待计时器回调

	AActor* ChooseRadomTarget(const TArray<AActor*>& TargetArray);

	UPROPERTY()
	AActor* SpawnPoint; // 记录动态生成的出生点
	UPROPERTY()
	AAIController* EnemyController;
	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*>PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMin = 4.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMax = 6.f;

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
