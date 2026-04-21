// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Actor Attributes")
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Actor Attributes")
	int32 GetGold() const { return Gold; }

protected:
	virtual void BeginPlay() override;

private:

public:	

protected:
	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	int32 Gold;

private:
	

};
