// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"

#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(RootComponent);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceStart->SetupAttachment(RootComponent);
	BoxTraceEnd->SetupAttachment(RootComponent);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponBox)
	{
		WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	}
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
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::SetCollision(ECollisionEnabled::Type CollisionType)
{
	WeaponBox->SetCollisionEnabled(CollisionType);
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

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	FVector BoxHalfExtent = FVector(5.0f,5.0f,5.0f);
	FHitResult HitPoint;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	for (AActor* ToIgnore : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(ToIgnore);
	}

	ActorsToIgnore.Add(GetOwner()); // 忽略拿武器的角色，防止砍到自己

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxHalfExtent, BoxTraceStart->GetComponentRotation(), UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitPoint, true);

	if (HitPoint.GetActor())
	{
		IHitInterface* HitInterface = Cast<IHitInterface>(HitPoint.GetActor());
		AActor* HitActor = HitPoint.GetActor();
		if (HitActor && HitActor->Implements<UHitInterface>())
		{
			IHitInterface::Execute_GetHit(HitPoint.GetActor(), HitPoint.ImpactPoint);
			IgnoreActors.AddUnique(HitPoint.GetActor());
		}
		
	}
}
