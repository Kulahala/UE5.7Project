// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/CharacterTypes.h"
#include "MyCharacter.generated.h"

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
	virtual void Tick(float DeltaTime) override;

	void Equip();
	void Attack();
	void ArmWeapon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	//播放攻击动画
	void PlayAttackMontage();

	//动画结束通知
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnArmMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//播放武器装备和卸下装备动画
	void PlayArmMontage(FName SectionName);

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* ArmMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	bool CanAttack() const;


private:
	UPROPERTY(VisibleInstanceOnly)
	Aitem* OverLapItem;

	UPROPERTY(VisibleInstanceOnly)
	AWeapon* EquippedWeapon;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_UnOccupied;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EArmWeaponState ArmWeaponState = EArmWeaponState::AWS_Disarming;

public:
	FORCEINLINE void SetEquippedItem(Aitem* Item) { OverLapItem = Item; }
	FORCEINLINE Aitem* GetEquippedItem() const { return OverLapItem; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE void SetActionState(EActionState NewState) { ActionState = NewState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE void SetArmWeaponState(EArmWeaponState NewState) { ArmWeaponState = NewState; }
	FORCEINLINE EArmWeaponState GetArmWeaponState() const { return ArmWeaponState; }
	FORCEINLINE AWeapon* GetWeapon()const { return EquippedWeapon; }
};