// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class AAIController;
class UHealthBarComponent;
class UWidgetComponent;
class UAttributeComponent;
class UNiagaraSystem;

UCLASS()
class TEST_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;
	double GetHitDirection(const FVector& Forward, const FVector& ToHit);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void DirectionalHitReact(const FVector& ImpactPoint, AActor* HitInstigator);
	void Die();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayHitReactMontage(const FName& SectionName);

	// 血条延迟隐藏机制
	FTimerHandle HealthBarHideTimer;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HealthBarDisplayTime = 4.0f;

	void ShowHealthBar();
	void HideHealthBar();

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Particle")
	UParticleSystem* HitParticle;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UHealthBarComponent* HealthBarWidgetComp;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_UnOccupied;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	//���Ŀ��
	UPROPERTY()
	AActor* CombatTarget;

	double CombatRadius = 500.f;

	/*
	 * Ѱ·
	 */
	//Ѳ��Ŀ��
	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly,Category = "Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "Navigation")
	TArray<AActor*>PatrolTargets;
public:
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
};
