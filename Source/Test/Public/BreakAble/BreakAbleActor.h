// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakAbleActor.generated.h"

UCLASS()
class TEST_API ABreakAbleActor : public AActor,public IHitInterface
{
	GENERATED_BODY()
	
public:	
	virtual void Tick(float DeltaTime) override;

	ABreakAbleActor();

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;


protected:
	virtual void BeginPlay() override;

	// 1. 碎前模型
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent * StaticMeshComp;

	// 2. 爆炸特效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Destruction")
	TSubclassOf<AActor> ImpactField;
public:	
	

};
