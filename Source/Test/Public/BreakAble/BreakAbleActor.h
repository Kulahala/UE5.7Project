#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakAbleActor.generated.h"

class ATreasure;
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

	//SM替换GC
	void BreakReplaced(const FVector& ImpactPoint);
	
	//破碎后放置宝物
	void SpawnSingleTreasure(UWorld* World);

	// --- 破碎后的表现配置 ---
	UPROPERTY(EditAnywhere, Category = "Destruction")
	UNiagaraSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Destruction")
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Destruction")
	float ExplosionForce = 500.f;

	UPROPERTY(EditAnywhere, Category = "Destruction")
	float DestructionLifeSpan = 3.0f; // 多少秒后回收整个 Actor

	UPROPERTY(EditAnywhere, Category = "Destruction")
	float ExplosionRadius = 300.f;

	UPROPERTY(EditAnywhere, Category = "Destruction|Drops")
	TSubclassOf<class ATreasure> BaseTreasureClass;

	// 可能会掉落的“物品卡片”池
	UPROPERTY(EditAnywhere, Category = "Destruction|Drops")
	TArray<class UTreasureData*> PossibleDrops;

	UPROPERTY(EditAnywhere, Category = "Destruction|Drops")
	int32 MinDrops = 1;

	UPROPERTY(EditAnywhere, Category = "Destruction|Drops")
	int32 MaxDrops = 3;

private:
	// 1. 完整的静态网格体
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComp;

	// 2. 碎裂用的几何体组件 (初始隐藏)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGeometryCollectionComponent* GCComp;

public:
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComp; }
	FORCEINLINE UGeometryCollectionComponent* GetGCComp() const { return GCComp; }
};
