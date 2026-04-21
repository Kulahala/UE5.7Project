// AnimNotify_AttachWeapon.h
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttachWeapon.generated.h"

UCLASS()
class TEST_API UAnimNotify_AttachWeapon : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

private:

public:
	// 允许你在动画编辑器中填入插槽名字！
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName SocketName;

protected:

private:
};