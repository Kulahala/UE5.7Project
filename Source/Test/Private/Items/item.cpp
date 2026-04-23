// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/item.h"

#include "Character/MyCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
Aitem::Aitem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ItemRoot"));
	SetRootComponent(ItemRoot);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(ItemRoot);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetGenerateOverlapEvents(true);
	Sphere->SetupAttachment(RootComponent);
	Sphere->InitSphereRadius(30.f);

	
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void Aitem::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	
	Sphere->OnComponentEndOverlap.AddDynamic(this, &Aitem::SphereEndOverlap);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &Aitem::SphereOverlap);
}

void Aitem::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex)
{
	if (AMyCharacter* SlashCharacter = Cast<AMyCharacter>(OtherActor))
	{
		SlashCharacter->SetEquippedItem(nullptr);
	}
}

void Aitem::SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* SlashCharacter = Cast<AMyCharacter>(OtherActor);
	if (SlashCharacter)
	{
		SlashCharacter->SetEquippedItem(this);
	}
}

void Aitem::StartSpawning(const FVector& Target)
{
	ItemState = EItemState::EIS_Spawning;
	TargetLocation = Target;
	StartLocation = GetActorLocation(); // 重新记录起点为当前位置
	SpawnRunningTime = 0.f;
}

void Aitem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Spawning)
	{
		SpawnRunningTime += DeltaTime;
		float Alpha = FMath::Clamp(SpawnRunningTime / SpawnDuration, 0.f, 1.f);
		
		// XY 轴线性插值
		FVector CurrentLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		
		// Z 轴叠加抛物线 (正弦波 0->1->0)
		CurrentLocation.Z += FMath::Sin(Alpha * PI) * SpawnHeight;
		
		SetActorLocation(CurrentLocation);

		// 抛物线结束
		if (Alpha >= 1.f)
		{
			ItemState = EItemState::EIS_Dropped;
			StartLocation = TargetLocation; // 更新掉落后的浮动基准点
			RunningTime = 0.f; // 重置浮动时间
		}
	}
	else if (ItemState == EItemState::EIS_Dropped)
	{
		// 计算Z轴绝对偏移，+1 保证最低点为初始位置，浮动区间为 [0, 2*Amplitude]
		float ZOffset = Amplitude * (FMath::Sin(RunningTime * TimeConstant) + 1.f);
		
		// 基于初始位置进行绝对位置更新
		SetActorLocation(StartLocation + FVector(0.f, 0.f, ZOffset));
	}

	// 无论是在抛物线中还是浮动中，都保持自转
	if (ItemState == EItemState::EIS_Spawning || ItemState == EItemState::EIS_Dropped)
	{
		AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
	}
}
