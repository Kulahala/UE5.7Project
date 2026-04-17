#include "Character/SlashAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MyCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	if (MyCharacter)
	{
		CharacterMovement = MyCharacter->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (FMath::IsNearlyZero(GroundSpeed))
	{
		IdleTime += DeltaSeconds;
	}
	else
	{
		IdleTime = 0;
	}

	if (!CharacterMovement || !MyCharacter) return;

	const FVector Velocity = CharacterMovement->Velocity;
	GroundSpeed = Velocity.Size2D();
	ZSpeed = Velocity.Z;

	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, MyCharacter->GetActorRotation());
	IsFalling = CharacterMovement->IsFalling();
		

	// 同步角色状态
	CharacterState = MyCharacter->GetCharacterState();
	ArmWeaponState = MyCharacter->GetArmWeaponState(); 

	
}
