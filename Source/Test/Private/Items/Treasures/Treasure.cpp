// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasures/Treasure.h"

#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure()
{
	PrimaryActorTick.bCanEverTick = true;

	if (ItemMesh) {
		ItemMesh->SetGenerateOverlapEvents(true);
		// 设置为仅查询，不进行物理碰撞计算
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// 忽略所有通道
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		// 单独让 Pawn 通道触发重叠事件
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void ATreasure::BeginPlay()
{
	Super::BeginPlay();
}

void ATreasure::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::SphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void ATreasure::SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult& SweepResult)
{
	if (AMyCharacter* SlashCharacter = Cast<AMyCharacter>(OtherActor))
	{
		if (PickSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickSound, OtherActor->GetActorLocation());
		}
		Destroy();
	}
}

void ATreasure::Tick(float DeltaSeconds)
{
}
