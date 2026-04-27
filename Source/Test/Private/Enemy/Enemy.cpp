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

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 330;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HealthBarWidgetComp->SetVisibility(false);
	// 绑定广播：当血量改变时，自动调用血条更新
	Attributes->OnHealthChanged.AddDynamic(HealthBarWidgetComp, &UHealthBarComponent::SetHealthPercent);
	// 初始化：设为满血
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
	MoveToPatrolTarget();
}


void AEnemy::GenerateNewLookRotation()
{
	float RandomYaw = FMath::RandRange(45.f, 120.f) * (FMath::RandBool() ? 1.f : -1.f);
	PatrolWaitTargetRotation = GetActorRotation() + FRotator(0.f, RandomYaw, 0.f);
}

void AEnemy::CheckPatrolTarget(float DeltaTime)
{
	if (!IsValid(PatrolTarget))return;
	if (BInTargetRange(PatrolTarget, PatrolRadius))
	{
		if (!GetWorldTimerManager().IsTimerActive(PatrolTimer))
		{
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
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, PatrolWaitTargetRotation, DeltaTime, PatrolRotationSpeed);
			SetActorRotation(NewRotation);
		}
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::EES_Dead)
	{
		return;
	}
	GroundSpeed = GetVelocity().Size2D();

	//状态切换逻辑
	switch (EnemyState)
	{
	case EEnemyState::EES_Patrolling:
		CheckPatrolTarget(DeltaTime);
		break;
	case EEnemyState::EES_Chasing: //只负责状态，判断交给CheckCombatTarget()
	case EEnemyState::EES_Attacking:
		//CheckCombatTarget();
		//战斗逻辑
		if (ChasingTarget)
		{
			if (BInTargetRange(ChasingTarget, ChasingRadius))
			{
				//TODO 在战斗半径内的逻辑
			}
			else
			{
				//超出战斗半径，清除目标
				EnemyState = EEnemyState::EES_Patrolling;
				ChasingTarget = nullptr;
				MoveToPatrolTarget();
			}
		}
		break;
	default:
		break;
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

double AEnemy::GetHitDirection(const FVector& Forward, const FVector& ToHit)
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
	Attributes->ReceiveDamage(DamageAmount);

	if (Attributes->IsAlive())
	{
		//没死
		AActor* DamagedActor = EventInstigator->GetPawn();
		if (DamagedActor)
		{
			ChasingTarget = DamagedActor;
			EnemyState = EEnemyState::EES_Chasing; // 被打了立刻进入追逐状态
		}
	}
	else //死了
	{
		if (EnemyController)
		{
			EnemyController->StopMovement();
		}

		//关闭胶囊体碰撞
		EnemyState = EEnemyState::EES_Dead;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetWorldTimerManager().ClearTimer(LookTimer);
		if (SpawnPoint)
		{
			SpawnPoint->Destroy();
		}

		SetLifeSpan(5.f);
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

void AEnemy::Die() //主要负责动画
{
	GetWorldTimerManager().ClearTimer(HealthBarHideTimer);
	HealthBarWidgetComp->SetVisibility(false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	//随机播放死亡动画
	if (AnimInstance && DeathMontage)
	{
		//强制打断其他动画
		AnimInstance->Montage_Stop(0.1f);

		int32 RandomIndex = FMath::RandRange(1, 3);
		FName SectionName = FName(FString::Printf(TEXT("Section%d"), RandomIndex));

		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator)
{
	//DrawDebugSphere(this->GetWorld(), ImpactPoint, 5, 10, FColor::Red, false, 5.0f, 0, 0.5f);

	ShowHealthBar();

	if (Attributes->IsAlive()) //存活
	{
		DirectionalHitReact(ImpactPoint, HitInstigator);
	}
	else //死亡
	{
		Die();
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

void AEnemy::MoveToPatrolTarget()
{
	if (EnemyController && PatrolTarget)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(PatrolTarget);

		float StopRadius = FMath::Max(15.f, PatrolRadius - 50.f);

		MoveRequest.SetAcceptanceRadius(StopRadius);
		if (StopRadius >= PatrolRadius)
		{
			ensureMsgf(false, TEXT("AEnemy: PatrolRadius (%.1f) 设定太小，请重新手动设定或程序自定设定"), PatrolRadius);
			StopRadius = FMath::Max(0.f, PatrolRadius - 10.f);
		}
		MoveRequest.SetUsePathfinding(true);
		FNavPathSharedPtr NavPath;
		EnemyController->MoveTo(MoveRequest, &NavPath);

		//调试绘制
		if (NavPath.IsValid())
		{
			TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
			for (FNavPathPoint& Point : PathPoints)
			{
				const FVector& PointLocation = Point.Location;
				DrawDebugSphere(GetWorld(), PointLocation, 12.f, 12, FColor::Green, false, 10.f);
			}
		}
	}
}

void AEnemy::PatrolTimerFinished()
{
	GetWorldTimerManager().ClearTimer(LookTimer); // 停止张望

	TArray<AActor*> Candidates; //额外目标数组
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			if (!BInTargetRange(Target, PatrolRadius))
			{
				Candidates.AddUnique(Target);
			}
		}
	}
	if ((Candidates.Num() > 0))
	{
		PatrolTarget = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
		MoveToPatrolTarget();
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
