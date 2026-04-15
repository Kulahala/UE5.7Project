#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 { ECS_Unequipped UMETA(DisplayName = "Unequipped"), ECS_OneHandEquipped UMETA(DisplayName = "One Hand Equipped"), ECS_TwoHandEquipped UMETA(DisplayName = "Two Hand Equipped") };

UENUM(BlueprintType)
enum class EActionState : uint8{ EAS_UnOccupied UMETA(DisplayName = "UnOccupied"), EAS_Attacking UMETA(DisplayName = "Attacking"),EAS_Arming UMETA(DisplayName = "Arming") };

UENUM(BlueprintType)
enum class EArmWeaponState : uint8 { AWS_Arming UMETA(DisplayName = "Arming"), AWS_Disarming UMETA(DisplayName = "Disarming") };
