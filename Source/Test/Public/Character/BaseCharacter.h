// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UAttributeComponent;
class AWeapon;

UCLASS()
class TEST_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;
	virtual void DirectionalHitReact(const FVector& ImpactPoint, const AActor* HitInstigator);

	virtual void Attack();
	virtual void Equip();

	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override; //受击逻辑

protected:
	//播放攻击动画
	virtual void PlayAttackMontage(const FName& SectionName);
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual bool CanAttack() const;
	//动画结束通知
	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//攻击动画
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EWeaponState CharacterState = EWeaponState::ECS_Unequipped;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EArmWeaponState ArmWeaponState = EArmWeaponState::AWS_Disarming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundBase* HitSound; //提取了

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* HitParticle; //提取了

	UPROPERTY()
	AWeapon* EquippedWeapon;

public:
	FORCEINLINE AWeapon* GetWeapon() const { return EquippedWeapon; }
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
	FORCEINLINE EWeaponState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EArmWeaponState GetArmWeaponState() const { return ArmWeaponState; }
};
