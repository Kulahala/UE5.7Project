// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakAble/ImpactEffect.h"

#include "NiagaraFunctionLibrary.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AImpactEffect::AImpactEffect()
{
	PrimaryActorTick.bCanEverTick = false;

	ExplosionForce = 500000.f; // 给个默认爆炸力度

	GCComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GCComp"));
	SetRootComponent(GCComp);


}

void AImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	if (ImpactParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactParticle, GetActorLocation());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (GCComp)
	{
		GCComp->SetSimulatePhysics(true);
		GCComp->AddRadialImpulse(GetActorLocation(), 200.f, ExplosionForce, RIF_Linear, true);
	}

	//x秒后回收碎片
	SetLifeSpan(3.0f);
}

void AImpactEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

