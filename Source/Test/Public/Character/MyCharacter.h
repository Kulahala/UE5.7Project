// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/CharacterTypes.h"
#include "MyCharacter.generated.h"

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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	//移动回调函数
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void MoveRight(float Value);
	void Equip();
	void Attack();

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	//播放攻击动画
	void PlayAttackMontage()const;

	//攻击动画蒙太奇
	UPROPERTY(EditDefaultsOnly,Category=Montages)
	UAnimMontage* AttackMontage;



private:
	UPROPERTY(VisibleInstanceOnly)
	Aitem* OverLapItem;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	EActionState ActionState = EActionState::EAS_UnOccupied;

public:
	FORCEINLINE void SetEquippedItem(Aitem* Item) { OverLapItem = Item; }
	FORCEINLINE Aitem* GetEquippedItem() const { return OverLapItem; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
