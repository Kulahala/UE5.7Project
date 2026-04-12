#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 { ECS_Unequipped UMETA(DisplayName = "Unequipped"), ECS_OneHandEquipped UMETA(DisplayName = "One Hand Equipped"), ECS_TwoHandEquipped UMETA(DisplayName = "Two Hand Equipped") };

UENUM(BlueprintType)
enum class EActionState : uint8{ EAS_UnOccupied UMETA(DisplayName = "UnOccupied"), EAS_Attacking UMETA(DisplayName = "Attacking") };
