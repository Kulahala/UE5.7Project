// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/item.h"
#include "Weapon.generated.h"

UCLASS()
class TEST_API AWeapon : public Aitem
{
	GENERATED_BODY()

public:
	AWeapon();
	void AttachMeshToSocket(USceneComponent* Parent, FName SocketName);
	void Equip(USceneComponent* Parent, FName SocketName);

	// 开始检测：重置旧位置
	void StartWeaponTrace();
	// 执行检测：每帧调用
	void ExecuteWeaponTrace();

	// 防重复受击黑名单
	UPROPERTY()
	TArray<AActor*> IgnoreActors;

protected:
	virtual void SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Weaponproperties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weaponproperties")
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Weaponproperties")
	USceneComponent* BoxTraceEnd;

	// 用于保存上一帧的位置，防止极高速穿模（扫面检测所需）
	FVector TraceStartOld;
	FVector TraceEndOld;
};
