#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Character/CharacterTypes.h" // 确保包含你的枚举头文件
#include "AnimNotify_SetState.generated.h"

// 专门用于改变 动作状态 (Action State) 的通知
UCLASS()
class TEST_API UAnimNotify_SetActionState : public UAnimNotify
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EActionState NewActionState = EActionState::EAS_UnOccupied; // 默认给个结束占用的状态

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// 专门用于改变 武器装备状态 (Arm Weapon State) 的通知
UCLASS()
class TEST_API UAnimNotify_SetArmWeaponState : public UAnimNotify
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EArmWeaponState NewArmState ;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};