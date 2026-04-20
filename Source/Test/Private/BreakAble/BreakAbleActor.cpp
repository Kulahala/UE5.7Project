// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakAble/BreakAbleActor.h"

ABreakAbleActor::ABreakAbleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	RootComponent = StaticMeshComp;

	StaticMeshComp->SetCollisionObjectType(ECC_Destructible);
	StaticMeshComp->SetGenerateOverlapEvents(true);

}

void ABreakAbleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakAbleActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (ImpactField != nullptr)
	{
		GetWorld()->SpawnActor<AActor>(ImpactField, GetActorLocation(), GetActorRotation());
	}
	Destroy();
}

void ABreakAbleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

