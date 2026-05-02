// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy.generated.h"

class UAIPerceptionComponent;
class AAIController;
class UHealthBarComponent;
class UWidgetComponent;

UCLASS()
class TEST_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	/* 生命周期 */
	AEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* 战斗 */
	virtual void Attack() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;
	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void DirectionalHitReact(const FVector& ImpactPoint, const AActor* HitInstigator) override;
	void Die(); // 死亡演出

	/* 蒙太奇结束回调 */
	UFUNCTION(BlueprintCallable)
	void OnHitReactEnd();
	UFUNCTION(BlueprintCallable)
	void OnAttackEnd();

protected:
	/* AI状态机 */
	void CheckCombatTarget(); // 持续检测战斗目标并切换状态
	void SetEnemyState(EEnemyState NewState); // 处理进入/退出状态的一次性事件
	void OnPatrolling(float DeltaTime); // 巡逻Tick逻辑
	void MoveToTarget(const AActor* Target); // 导航移动到目标
	bool BInTargetRange(AActor* Target, double Range) const; // 检查目标是否在范围内

	/* 蒙太奇 */
	virtual bool CanAttack() const override;
	virtual void PlayHitReactMontage(const FName& SectionName) override;

	/* 血条显示 */
	void ShowHealthBar();
	void HideHealthBar();
	FTimerHandle HealthBarHideTimer; // 血条延迟隐藏定时器

	/* AI感知 */
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus); // 感知到目标的回调

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HealthBarDisplayTime = 4.0f; // 血条显示持续时间

private:
	/* 组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UHealthBarComponent* HealthBarWidgetComp;

	/* 状态 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_UnOccupied;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed; // 动画蓝图用的地速

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float Direction; // 移动方向（-180~180，用于 BlendSpace）


	/* 战斗属性 */
	UPROPERTY(VisibleInstanceOnly)
	AActor* ChasingTarget; // 当前追击目标

	// 感知/追击范围：玩家进入此距离 → 追击；超出 → 丢失目标回到巡逻
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingRadius = 1000.f;

	// 战斗范围：目标进入此距离 → 停止追击，开始攻击
	// 需大于 AI 实际能靠近的最小距离（受导航网格和胶囊体碰撞影响）
	UPROPERTY(EditAnywhere, Category = "Combat")
	float CombatingRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> WeaponClass; // 选择武器类，BeginPlay自动生成

	/* 巡逻 */
	UPROPERTY()
	AActor* SpawnPoint; // 动态生成的出生点

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget; // 当前巡逻目标点

	// 巡逻到达判定半径：进入此距离 → 视为到达巡逻点，开始等待/张望
	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> PatrolTargets; // 巡逻点列表

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMin = 4.f; // 到达巡逻点后最短等待时间

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMax = 6.f; // 到达巡逻点后最长等待时间

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolRotationSpeed = 2.f; // 巡逻张望旋转速度

	UPROPERTY(EditAnywhere, Category = "Ai Navigation", meta = (AllowPrivateAccess = "true"))
	float SingleLookTime = 1.5f; // 单次张望持续时间

	FTimerHandle PatrolTimer; // 巡逻等待定时器
	FTimerHandle LookTimer; // 张望定时器
	void PatrolTimerFinished(); // 等待结束回调
	void ClearPatrolTimers(); // 清理巡逻相关定时器
	void GenerateNewLookRotation(); // 生成新的张望方向
	AActor* ChooseRadomTarget(const TArray<AActor*>& TargetArray); // 随机选择巡逻点
	FRotator PatrolWaitTargetRotation; // 张望目标旋转
};
