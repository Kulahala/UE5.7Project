// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/item.h"
#include "Weapon.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class TEST_API AWeapon : public Aitem
{
	GENERATED_BODY()
	
public:
	AWeapon();
	void AttachMeshToSocket(USceneComponent* Parent, FName SocketName);
	void Equip(USceneComponent* Parent,FName SocketName);

protected:
	virtual void ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;

	virtual void ItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

private:
	UPROPERTY(EditAnywhere,Category="Weaponproperties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere,Category="Weaponproperties")
	UBoxComponent* WeaponBox;


};
