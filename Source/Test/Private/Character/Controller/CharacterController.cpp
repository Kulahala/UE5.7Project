// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Controller/CharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" 

void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterController::Input_Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterController::Input_Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacterController::Input_Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacterController::Input_StopJumping);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ACharacterController::Input_Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACharacterController::Input_Attack);
	}
}

void ACharacterController::Input_Move(const FInputActionValue& Value)
{
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetPawn());
	if (!MyCharacter) return;

	if (MyCharacter->GetActionState() == EActionState::EAS_Attacking) return;
	if (MyCharacter->GetCharacterMovement()->IsFalling()) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator CurrentControlRotation = GetControlRotation();
	const FRotator YawRotation(0, ControlRotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	MyCharacter->AddMovementInput(ForwardDirection, MovementVector.X); 
	MyCharacter->AddMovementInput(RightDirection, MovementVector.Y);   
}

void ACharacterController::Input_Look(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACharacterController::Input_Jump()
{
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetPawn()))
	{
		MyCharacter->Jump();
	}
}

void ACharacterController::Input_StopJumping()
{
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetPawn()))
	{
		MyCharacter->StopJumping();
	}
}

void ACharacterController::Input_Equip()
{
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetPawn()))
	{
		MyCharacter->Equip(); 
	}
}

void ACharacterController::Input_Attack()
{
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetPawn()))
	{
		MyCharacter->Attack(); 
	}
}