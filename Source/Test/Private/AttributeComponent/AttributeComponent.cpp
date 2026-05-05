// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Gold = 0;
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	
	OnHealthChanged.Broadcast(GetHealthPercent());
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 耐力冷却倒计时
	if (StaminaRegenCooldown > 0.f)
	{
		StaminaRegenCooldown -= DeltaTime;
	}

	// 耐力自然恢复（未暂停 且 冷却结束后）
	if (!bStaminaRegenPaused && StaminaRegenCooldown <= 0.f && CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
		OnStaminaChanged.Broadcast(GetStaminaPercent());
	}
}

void UAttributeComponent::UseStamina(float Amount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.f, MaxStamina);
	OnStaminaChanged.Broadcast(GetStaminaPercent());
}

void UAttributeComponent::ResetStaminaRegenCooldown()
{
	StaminaRegenCooldown = StaminaRegenDelay;
}

void UAttributeComponent::PauseStaminaRegen()
{
	bStaminaRegenPaused = true;
}

void UAttributeComponent::ResumeStaminaRegen()
{
	bStaminaRegenPaused = false;
	StaminaRegenCooldown = StaminaRegenDelay;
}
