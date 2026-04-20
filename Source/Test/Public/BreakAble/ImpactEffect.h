// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImpactEffect.generated.h"

class UNiagaraSystem;

UCLASS()
class TEST_API AImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	AImpactEffect();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Destruction")
	UGeometryCollectionComponent* GCComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destruction")
	UNiagaraSystem* ImpactParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destruction")
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destruction")
	float ExplosionForce;
public:	
	

};
