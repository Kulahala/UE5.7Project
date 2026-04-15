// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"

#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(RootComponent);

}

void AWeapon::AttachMeshToSocket(USceneComponent* Parent, FName SocketName)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(Parent, AttachmentRules, SocketName);
}

void AWeapon::Equip(USceneComponent* Parent, FName SocketName)
{
	AttachMeshToSocket(Parent, SocketName);
	ItemState = EItemState::EIS_Equipped;
	if (!EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
	if (ItemMesh)
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::ItemEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::ItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::ItemOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
