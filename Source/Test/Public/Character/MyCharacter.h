// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/CharacterTypes.h"
#include "MyCharacter.generated.h"

class UAttributeComponent;
class AWeapon;
class Aitem;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TEST_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	void Equip();
	void Attack();
	void ArmWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//播放攻击动画
	void PlayAttackMontage();

	//动画结束通知
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnArmMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//播放武器装备和卸下装备动画
	void PlayArmMontage(const FName& SectionName);

	bool CanAttack() const;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* ArmMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleInstanceOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	Aitem* OverLapItem;

	UPROPERTY(VisibleInstanceOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_UnOccupied;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EArmWeaponState ArmWeaponState = EArmWeaponState::AWS_Disarming;

public:
	FORCEINLINE void SetEquippedItem(Aitem* Item) { OverLapItem = Item; }
	FORCEINLINE Aitem* GetEquippedItem() const { return OverLapItem; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE void SetActionState(const EActionState NewState) { ActionState = NewState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE void SetArmWeaponState(const EArmWeaponState NewState) { ArmWeaponState = NewState; }
	FORCEINLINE EArmWeaponState GetArmWeaponState() const { return ArmWeaponState; }
	FORCEINLINE AWeapon* GetWeapon() const { return EquippedWeapon; }
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
};
