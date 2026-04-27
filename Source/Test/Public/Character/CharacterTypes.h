#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"), ECS_OneHandEquipped UMETA(DisplayName = "One Hand Equipped"),
	ECS_TwoHandEquipped UMETA(DisplayName = "Two Hand Equipped")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_UnOccupied UMETA(DisplayName = "UnOccupied"), EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Arming UMETA(DisplayName = "Arming"), EAC_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EArmWeaponState : uint8
{
	AWS_Arming UMETA(DisplayName = "Arming"), AWS_Disarming UMETA(DisplayName = "Disarming")
};

UENUM()
enum class EEnemyState : uint8 {
	EES_Patrolling UMETA(DisplayName = "Patrolling"), // 巡逻中
	EES_Chasing UMETA(DisplayName = "Chasing"),       // 追逐中
	EES_Attacking UMETA(DisplayName = "Attacking"),   // 攻击中
	EES_Dead UMETA(DisplayName = "Dead")              // 已死亡
};
