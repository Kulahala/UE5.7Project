// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "item.generated.h"

UCLASS()
class TEST_API Aitem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Aitem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float deltatime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UFUNCTION()
	virtual void ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void ItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Amplitude = 50.0f; // 浮动的高度范围

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TimeConstant = 2.0f; // 浮动的速度频率
};
