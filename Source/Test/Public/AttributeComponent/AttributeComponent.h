// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, HealthPercent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	void ReceiveDamage(float Damage);


protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes", meta = (AllowPrivateAccess = "true"))
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

public:
	UFUNCTION(BlueprintCallable, Category = "Actor Attributes")
	FORCEINLINE void AddGold(int32 Amount) { Gold += Amount; }

	UFUNCTION(BlueprintCallable, Category = "Actor Attributes")
	FORCEINLINE void SetGold(int32 Value) { Gold = Value; }

	UFUNCTION(BlueprintPure, Category = "Actor Attributes")
	FORCEINLINE int32 GetGold() const { return Gold; }

	FORCEINLINE float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

	FORCEINLINE bool IsAlive() const { return CurrentHealth > 0.f; }
};
