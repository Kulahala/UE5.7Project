// ABreakAbleActor.cpp
#include "BreakAble/BreakAbleActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ABreakAbleActor::ABreakAbleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 创建完整模型
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	RootComponent = StaticMeshComp;
	StaticMeshComp->SetCollisionObjectType(ECC_Destructible);

	// 创建碎裂模型
	GCComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GCComp"));
	GCComp->SetupAttachment(RootComponent); // 挂载到根部

	// 初始状态：隐藏且关闭物理
	GCComp->SetVisibility(false);
	GCComp->SetSimulatePhysics(false);
	GCComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 初始关闭碰撞，防止干扰
}

void ABreakAbleActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakAbleActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator)
{
	// --- 1. 状态切换 ---
	// 隐藏并关闭完整模型的碰撞
	StaticMeshComp->SetVisibility(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 显示碎裂模型并开启物理
	GCComp->SetVisibility(true);
	GCComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GCComp->SetSimulatePhysics(true);

	// --- 2. 施加冲击力 ---
	GCComp->AddRadialImpulse(ImpactPoint, 200.f, ExplosionForce, ERadialImpulseFalloff::RIF_Linear, true);

	// --- 3. 播放效果 ---
	if (ImpactParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactParticle, ImpactPoint);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactPoint);
	}
	//延迟销毁
	SetLifeSpan(DestructionLifeSpan);
}