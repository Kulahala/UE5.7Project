// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapon/Weapon.h"
#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(GetMesh());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(GetMesh());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Parent, AttachmentRules, SocketName);
}

void AWeapon::Equip(USceneComponent* Parent, const FName& SocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttachMeshToSocket(Parent, SocketName);
	ItemState = EItemState::EIS_Equipped;

	if (GetEffect())
	{
		GetEffect()->Deactivate();
	}

	// 非常关键：绑定武器的主人，避免砍中自己
	if (Parent && Parent->GetOwner())
	{
		SetOwner(Parent->GetOwner());
	}

	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::StartWeaponTrace()
{
	// 记录攻击刚开始时的初始位置
	if (BoxTraceStart && BoxTraceEnd)
	{
		TraceStartOld = BoxTraceStart->GetComponentLocation();
		TraceEndOld = BoxTraceEnd->GetComponentLocation();
	}
}

void AWeapon::ExecuteWeaponTrace()
{
	if (!BoxTraceStart || !BoxTraceEnd)
	{
		return;
	}

	FVector Start = BoxTraceStart->GetComponentLocation();
	FVector End = BoxTraceEnd->GetComponentLocation();

	// 1. 计算当前帧的刀身中心，以及上一帧的刀身中心
	FVector CurrentCenter = (Start + End) / 2.0f;
	FVector OldCenter = (TraceStartOld + TraceEndOld) / 2.0f;

	// 2. 构造代表整把刀的盒子
	float HalfLength = FVector::Distance(Start, End) / 2.0f;
	FVector BoxHalfExtent = FVector(HalfLength, 5.0f, 5.0f);

	// 3. 获取刀的朝向（从刀柄指向刀尖）
	FRotator TraceRotation = (End - Start).Rotation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	if (GetOwner())
	{
		ActorsToIgnore.AddUnique(GetOwner());
	}

	for (AActor* ToIgnore : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(ToIgnore);
	}

	FHitResult HitPoint;

	// 4. 真正的扫面：让代表整把刀的盒子，从上一帧的中心，扫到当前帧的中心！
	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this, OldCenter, CurrentCenter, BoxHalfExtent, TraceRotation,
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, ActorsToIgnore, EDrawDebugTrace::None, HitPoint,
		true, FLinearColor::Red, FLinearColor::Green, 3.f);

	// 记录本帧位置，留给下帧做参考
	TraceStartOld = Start;
	TraceEndOld = End;

	if (bHit && HitPoint.GetActor())
	{
		AActor* HitActor = HitPoint.GetActor();

		UGameplayStatics::ApplyDamage(HitPoint.GetActor(), Damage, GetInstigatorController(), this,
			UDamageType::StaticClass());

		if (HitActor->Implements<UHitInterface>())
		{
			IHitInterface::Execute_GetHit(HitActor, HitPoint.ImpactPoint, GetOwner());
		}

		// 击中一次后加入黑名单，防止同一刀造成多次伤害
		IgnoreActors.AddUnique(HitActor);
	}
}

void AWeapon::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::SphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::SphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
