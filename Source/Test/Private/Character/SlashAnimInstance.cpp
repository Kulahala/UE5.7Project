#include "Character/SlashAnimInstance.h"
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
	IsFalling = CharacterMovement->IsFalling();
	ZSpeed = Velocity.Z;

	// 同步角色状态
	CharacterState = MyCharacter->GetCharacterState();
	ArmWeaponState = MyCharacter->GetArmWeaponState(); // <-- 新增这一行，确保动画蓝图能拿到最新状态
}
