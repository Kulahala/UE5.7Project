// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/item.h"
#include "Treasure.generated.h"


UCLASS()
class TEST_API ATreasure : public Aitem
{
	GENERATED_BODY()
public:
	ATreasure();
	virtual  void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;

	virtual void SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;

	virtual void SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* PickSound;

};
