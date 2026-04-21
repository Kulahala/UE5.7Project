// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/item.h"

#include "Character/MyCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
Aitem::Aitem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetGenerateOverlapEvents(true);
	Sphere->SetupAttachment(RootComponent);
	Sphere->InitSphereRadius(30.f);

	
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void Aitem::BeginPlay()
{
	Super::BeginPlay();
	
	// 记录初始位置
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

void Aitem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Dropped)
	{
		// 计算Z轴绝对偏移，+1 保证最低点为初始位置，浮动区间为 [0, 2*Amplitude]
		float ZOffset = Amplitude * (FMath::Sin(RunningTime * TimeConstant) + 1.f);
		
		// 基于初始位置进行绝对位置更新
		SetActorLocation(StartLocation + FVector(0.f, 0.f, ZOffset));
	}
}
