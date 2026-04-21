// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "item.generated.h"

class USphereComponent;

UENUM()
enum class EItemState : int8{ EIS_Dropped,EIS_Equipped };

UCLASS()
class TEST_API Aitem : public AActor
{
	GENERATED_BODY()
	
public:
	Aitem();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

public:

protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Amplitude = 10.f; // 浮动的高度范围

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TimeConstant = 4.0f; // 浮动的速度频率

	UPROPERTY(BlueprintReadOnly)
	EItemState ItemState = EItemState::EIS_Dropped;

	float RunningTime = 0.f;

	// 记录初始位置，作为浮动的基准点
	FVector StartLocation;

	float deltatime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* ItemRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Sphere;

private:
};
