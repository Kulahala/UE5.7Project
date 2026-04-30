// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "MyCharacter.generated.h"

class Aitem;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TEST_API AMyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Equip() override;
	void ArmWeapon();
	void Sprint();
	void StopSprinting();
	void Walk();
	void StopWalking();

	virtual void Attack() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitInstigator) override;

protected:
	//动画结束通知
	void OnArmMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

	//播放武器装备和卸下装备动画
	void PlayArmMontage(const FName& SectionName);
	//播放主角攻击动画
	virtual void PlayAttackMontage(const FName& SectionName) override;
	virtual bool CanAttack() const override;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* ArmMontage;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_UnOccupied;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleInstanceOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	Aitem* OverLapItem;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsWalking = false;

public:
	/*
	 *Getters and Setters
	 */
	FORCEINLINE void SetEquippedItem(Aitem* Item) { OverLapItem = Item; }
	FORCEINLINE Aitem* GetEquippedItem() const { return OverLapItem; }
	FORCEINLINE void SetActionState(const EActionState NewState) { ActionState = NewState; }
	FORCEINLINE void SetArmWeaponState(const EArmWeaponState NewState) { ArmWeaponState = NewState; }
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
