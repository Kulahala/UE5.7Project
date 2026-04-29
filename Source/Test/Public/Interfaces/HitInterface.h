// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

class TEST_API IHitInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void GetHit(const FVector& ImpactPoint, AActor* HitInstigator);

	// 纯数学工具：通过点积+叉积计算受击角度，返回 [-180, 180]
	static double GetHitDirection(const FVector& Forward, const FVector& ToHit)
	{
		const double CosTheta = FVector::DotProduct(Forward, ToHit);
		double Theta = FMath::RadiansToDegrees(FMath::Acos(CosTheta));
		const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
		if (CrossProduct.Z < 0)
		{
			Theta *= -1.f;
		}
		return Theta;
	}
};
