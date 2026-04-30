// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "AttributeComponent/AttributeComponent.h"
#include "components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/BaseHealthBarWidget.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

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

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = ChasingRadius;
	SightConfig->LoseSightRadius = ChasingRadius + 200.f;
	SightConfig->PeripheralVisionAngleDegrees = 40.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 150;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator)
{
	//DrawDebugSphere(this->GetWorld(), ImpactPoint, 5, 10, FColor::Red, false, 5.0f, 0, 0.5f);

	ShowHealthBar();

	if (Attributes->IsAlive()) //存活
	{
		DirectionalHitReact(ImpactPoint, HitInstigator);
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


float AEnemy::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator,
                         AActor* DamageCauser)
{
	Attributes->ReceiveDamage(DamageAmount);

	if (Attributes->IsAlive())
	{
		//没死
		AActor* DamagedActor = EventInstigator->GetPawn();
		if (DamagedActor)
		{
			ChasingTarget = DamagedActor;

			// 被打了脑子宕机，进入硬直状态。清理计时器等操作全部交由 SetEnemyState 处理
			SetEnemyState(EEnemyState::EES_Stunned);
		}
	}
	else
	{	//死了
		SetEnemyState(EEnemyState::EES_Dead);
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint, const AActor* HitInstigator)
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
	double Theta = IHitInterface::GetHitDirection(Forward, ToHit);
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

void AEnemy::Die() //主要负责动画和死亡相关底层逻辑
{
	ClearPatrolTimers();
	GetWorldTimerManager().ClearTimer(HealthBarHideTimer);

	// 2. 停止移动
	if (EnemyController)EnemyController->StopMovement();

	// 3. 关闭碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 4. 清理附加物和UI
	if (SpawnPoint)
	{
		SpawnPoint->Destroy();
	}
	HealthBarWidgetComp->SetVisibility(false);

	// 5. 播放死亡动画
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		//强制打断其他动画
		AnimInstance->Montage_Stop(0.1f);

		int32 RandomIndex = FMath::RandRange(1, 3);
		FName SectionName = FName(FString::Printf(TEXT("Section%d"), RandomIndex));

		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}

	// 6. 设定销毁时间
	SetLifeSpan(5.f);
}

// 兜底：定时器全量清理，覆盖 Die() 未执行的路径（关卡切换、编辑器 Stop 等）
void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearPatrolTimers();
	GetWorldTimerManager().ClearTimer(HealthBarHideTimer);
	Super::EndPlay(EndPlayReason);
}

void AEnemy::OnHitReactEnd()
{
	// 只有在硬直状态下才恢复，防止覆盖了死亡状态
	if (EnemyState == EEnemyState::EES_Stunned)
	{
		SetEnemyState(EEnemyState::EES_Chasing); // 恢复追击，下一帧 Tick 就会重新测距
	}
}

void AEnemy::OnAttackEnd()
{
	// 只有在攻击状态下才恢复，防止覆盖了受击状态或死亡状态
	if (EnemyState == EEnemyState::EES_Attacking)
	{
		SetEnemyState(EEnemyState::EES_Chasing); // 恢复追击，下一帧 Tick 就会重新测距
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidgetComp)
	{
		HealthBarWidgetComp->SetVisibility(false);
	}
	if (Attributes)
	{
		// 绑定广播：当血量改变时，自动调用血条更新
		Attributes->OnHealthChanged.AddDynamic(HealthBarWidgetComp, &UHealthBarComponent::SetHealthPercent);
	}
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::TargetPerceptionUpdated);
	}
	
	HealthBarWidgetComp->SetHealthPercent(Attributes->GetHealthPercent());

	// 添加出生点（将Z轴坐标下移到胶囊体底部，防止悬空导致后续寻路失败）
	FVector SpawnLocation = GetActorLocation();
	if (GetCapsuleComponent())
	{
		SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	SpawnPoint = GetWorld()->SpawnActor<ATargetPoint>(ATargetPoint::StaticClass(), SpawnLocation, GetActorRotation());
	if (SpawnPoint)
	{
		PatrolTargets.Add(SpawnPoint);
		if (!PatrolTarget)
		{
			PatrolTarget = SpawnPoint;
		}
	}

	EnemyController = Cast<AAIController>(GetController());
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::EES_Dead) return;

	// 在播受击动画、或者在播攻击动画期间，直接锁死，什么脑子和手脚都不准动
	if (EnemyState == EEnemyState::EES_Stunned || EnemyState == EEnemyState::EES_Attacking) return;

	GroundSpeed = GetVelocity().Size2D();

	// 1. 初步判断
	CheckCombatTarget();

	// 2. 持续反应
	switch (EnemyState)
	{
	case EEnemyState::EES_Patrolling:
		OnPatrolling(DeltaTime);
		break;
	case EEnemyState::EES_Chasing:
		// 玩家跑出网格体边缘时，AI 会走到边缘并停下（变为 Idle 状态）
		// 如果此时玩家又回来了，我们需要重新激活寻路指令
		if (EnemyController && EnemyController->GetMoveStatus() == EPathFollowingStatus::Idle)
		{
			MoveToTarget(ChasingTarget);
		}
		break;
	case EEnemyState::EES_Combating:
		// TODO 执行攻击行为 (比如 StartAttack() 播放挥剑动画)
		//OnCombating();
		break;
	default:
		break;
	}
}

void AEnemy::OnPatrolling(float DeltaTime)
{
	if (!IsValid(PatrolTarget)) return;
	if (BInTargetRange(PatrolTarget, PatrolRadius))
	{
		if (!GetWorldTimerManager().IsTimerActive(PatrolTimer))
		{
			// 刚到达：停止移动，关闭自动朝向运动方向，准备张望
			if (EnemyController) EnemyController->StopMovement();
			GetCharacterMovement()->bOrientRotationToMovement = false;

			// 刚到达：启动计时器
			const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
			GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);

			// 启动单次转身的循环计时器
			GetWorldTimerManager().SetTimer(LookTimer, this, &AEnemy::GenerateNewLookRotation, SingleLookTime, true);

			// 立即生成第一个转身目标
			GenerateNewLookRotation();
		}
		else
		{
			// 等待期间：平滑旋转到当前决定的目标朝向
			FRotator CurrentRotation = GetActorRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, PatrolWaitTargetRotation, DeltaTime,
			                                        PatrolRotationSpeed);
			SetActorRotation(NewRotation);
		}
	}
}

void AEnemy::SetEnemyState(EEnemyState NewState)
{
	if (EnemyState == NewState) return;

	// --- 退出旧状态的逻辑 ---
	if (EnemyState == EEnemyState::EES_Patrolling)
	{
		ClearPatrolTimers();
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	// --- 切换状态 ---
	EnemyState = NewState;

	// --- 进入新状态的逻辑（一次性动作） ---
	switch (EnemyState)
	{
	case EEnemyState::EES_Chasing:
		GetCharacterMovement()->MaxWalkSpeed = 330;
		// 切入追击瞬间，只需下达一次寻路指令
		MoveToTarget(ChasingTarget);
		break;
	case EEnemyState::EES_Combating:
		// 切入战斗状态瞬间，停止寻路移动，准备攻击或绕前
		if (EnemyController) EnemyController->StopMovement();
		break;
	case EEnemyState::EES_Attacking:
		// 攻击硬直状态，确保不移动
		if (EnemyController) EnemyController->StopMovement();
		break;
	case EEnemyState::EES_Patrolling:
		GetCharacterMovement()->MaxWalkSpeed = 150;
		MoveToTarget(PatrolTarget);
		break;
	case EEnemyState::EES_Stunned:
		// 被打出硬直瞬间，立刻刹车防止滑步
		if (EnemyController) EnemyController->StopMovement();
		break;
	case EEnemyState::EES_Dead:
		Die();
		break;
	default:
		break;
	}
}

void AEnemy::CheckCombatTarget()
{
	// 如果没有玩家，或者玩家死了，切回巡逻
	if (!ChasingTarget)
	{
		SetEnemyState(EEnemyState::EES_Patrolling);
		return;
	}
	//进入战斗范围
	if (BInTargetRange(ChasingTarget, CombatingRadius))
	{
		SetEnemyState(EEnemyState::EES_Combating);
	}
	//不在战斗范围，在追逐范围
	else if (BInTargetRange(ChasingTarget, ChasingRadius))
	{
		SetEnemyState(EEnemyState::EES_Chasing);
	}
	else
	{
		// 跑太远追丢了
		ChasingTarget = nullptr;
		SetEnemyState(EEnemyState::EES_Patrolling);
	}
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

bool AEnemy::BInTargetRange(AActor* Target, double Range) const
{
	if (!IsValid(Target))
	{
		return false;
	}
	double Distance = (Target->GetActorLocation() - GetActorLocation()).SizeSquared2D();
	return Distance <= FMath::Square(Range);
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidgetComp)
	{
		HealthBarWidgetComp->SetVisibility(true);
	}

	// 每次调用都会重置 4 秒倒计时
	GetWorldTimerManager().SetTimer(
		HealthBarHideTimer,
		this,
		&AEnemy::HideHealthBar,
		HealthBarDisplayTime,
		false
	);
}

void AEnemy::HideHealthBar()
{
	// TODO: 魂系锁定机制。等待锁定功能实现后，在这里判断 if (bIsTargetedByPlayer) return; 以保持血条显示

	if (HealthBarWidgetComp)
	{
		// 获取组件内部的 Widget 实例，并尝试转换为我们的 C++ 基类
		UBaseHealthBarWidget* HealthWidget = Cast<UBaseHealthBarWidget>(HealthBarWidgetComp->GetUserWidgetObject());

		if (HealthWidget)
		{
			// 触发蓝图事件：播放动画 -> 延迟 0.5s -> 隐藏 -> 恢复透明度
			HealthWidget->PlayFadeOutAnim();
		}
		else
		{
			// 如果强转失败或还没绑定蓝图，降级处理为直接隐藏
			HealthBarWidgetComp->SetVisibility(false);
		}
	}
}

void AEnemy::MoveToTarget(const AActor* Target)
{
	if (!EnemyController || !Target)return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);

	float StopRadius = FMath::Max(15.f, PatrolRadius - 50.f);

	MoveRequest.SetAcceptanceRadius(StopRadius);
	if (StopRadius >= PatrolRadius)
	{
		ensureMsgf(false, TEXT("AEnemy: PatrolRadius (%.1f) 设定太小，请重新手动设定或程序自定设定"), PatrolRadius);
		StopRadius = FMath::Max(0.f, PatrolRadius - 10.f);
	}
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAllowPartialPath(true);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChooseRadomTarget(const TArray<AActor*>& TargetArray)
{
	TArray<AActor*> Candidates;
	for (AActor* Target : TargetArray)
	{
		if (Target == PatrolTarget)continue;
		if (!BInTargetRange(Target, PatrolRadius))
		{
			Candidates.AddUnique(Target);
		}
	}

	if (Candidates.Num() > 0)
	{
		return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	}

	return nullptr;
}

void AEnemy::PatrolTimerFinished()
{
	GetWorldTimerManager().ClearTimer(LookTimer); // 停止张望
	GetCharacterMovement()->bOrientRotationToMovement = true; // 恢复自动朝向运动方向

	if (AActor* Target = ChooseRadomTarget(PatrolTargets))
	{
		PatrolTarget = Target;
		MoveToTarget(PatrolTarget);
	}
	else
	{
		// 极端情况：所有点都挨得太近了，原地重新开启一轮等待和张望
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
		GetWorldTimerManager().SetTimer(LookTimer, this, &AEnemy::GenerateNewLookRotation, SingleLookTime, true);
		GenerateNewLookRotation();
	}
}

void AEnemy::ClearPatrolTimers()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	GetWorldTimerManager().ClearTimer(LookTimer);
}

void AEnemy::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 只处理“看到”的瞬间
	if (!Stimulus.WasSuccessfullySensed()) return;
	// 身份校验
	if (!Actor || !Actor->ActorHasTag(FName("Player")))return;
	// 冗余校验
	if (ChasingTarget == Actor) return;
	// 专注度校验
	if (EnemyState == EEnemyState::EES_Chasing || EnemyState == EEnemyState::EES_Attacking) return;
	// 锁定新目标
	APawn* SeenPawn = dynamic_cast<APawn*>(Actor);
	ChasingTarget = SeenPawn;
}

void AEnemy::GenerateNewLookRotation()
{
	float RandomYaw = FMath::RandRange(45.f, 120.f) * (FMath::RandBool() ? 1.f : -1.f);
	PatrolWaitTargetRotation = GetActorRotation() + FRotator(0.f, RandomYaw, 0.f);
}
