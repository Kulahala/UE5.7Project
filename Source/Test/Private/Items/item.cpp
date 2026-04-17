// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/item.h"

#include "Character/MyCharacter.h"

// Sets default values
Aitem::Aitem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
}

// Called when the game starts or when spawned
void Aitem::BeginPlay()
{
	Super::BeginPlay();
	ItemMesh->OnComponentEndOverlap.AddDynamic(this, &Aitem::ItemEndOverlap);
	ItemMesh->OnComponentBeginOverlap.AddDynamic(this, &Aitem::ItemOverlap);
}

void Aitem::ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex)
{
	if (AMyCharacter* SlashCharacter = Cast<AMyCharacter>(OtherActor))
	{
		SlashCharacter->SetEquippedItem(nullptr);
	}
}

void Aitem::ItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* SlashCharacter = Cast<AMyCharacter>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetEquippedItem(this);
	}
}

// Called every frame
void Aitem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Dropped)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, Amplitude * FMath::Sin(RunningTime * TimeConstant)));
	}
}
