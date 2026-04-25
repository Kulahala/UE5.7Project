#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TreasureData.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UTreasureData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Treasure Info")
	UStaticMesh* TreasureMesh;

	UPROPERTY(EditAnywhere, Category = "Treasure Info")
	int32 GoldValue = 10;
	
	UPROPERTY(EditAnywhere, Category = "Treasure Info")
	USoundBase* PickUpSound;

	UPROPERTY(EditAnywhere, Category = "Treasure Info")
	FString TreasureName = TEXT("宝物");

	UPROPERTY(EditAnywhere, Category = "Treasure Info")
	float TreasureScale = 1.f;

protected:

private:

};
