#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakAbleActor.generated.h"

class UGeometryCollectionComponent;
class UNiagaraSystem;

UCLASS()
class TEST_API ABreakAbleActor : public AActor, public IHitInterface
{
	GENERATED_BODY()

public:
	ABreakAbleActor();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;

protected:
	virtual void BeginPlay() override;

	// 1. 完整的静态网格体
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComp;

	// 2. 碎裂用的几何体组件 (初始隐藏)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGeometryCollectionComponent* GCComp;

	// --- 破碎后的表现配置 ---
	UPROPERTY(EditAnywhere, Category = "Destruction")
	UNiagaraSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Destruction")
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Destruction")
	float ExplosionForce = 500.f; // 这里的力度根据 GC 的质量来调，建议先给小一点

	UPROPERTY(EditAnywhere, Category = "Destruction")
	float DestructionLifeSpan = 3.0f; // 多少秒后回收整个 Actor
};