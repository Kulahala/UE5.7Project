// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterTypes.h"
#include "SlashAnimInstance.generated.h"

class UCharacterMovementComponent;
class AMyCharacter;

/**
 * 
 */
UCLASS()
class TEST_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	AMyCharacter* MyCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float ZSpeed;

	UPROPERTY(BlueprintReadOnly, Category="Movement|Character")
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float IdleTime;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void AnimNotify_AttackEnd() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void AnimNotify_ArmEnd() const;


};
