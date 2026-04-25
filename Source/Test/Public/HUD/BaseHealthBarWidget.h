// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseHealthBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class TEST_API UBaseHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthPercent(float Percent);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_Health;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_Buffer;

private:
	// 缓冲条追赶真实血条的速度
	UPROPERTY(EditAnywhere, Category = "Health Bar")
	float BufferInterpSpeed = 3.0f;

	// 缓冲条开始下降前的等待时间（延迟时间）
	UPROPERTY(EditAnywhere, Category = "Health Bar")
	float BufferDelayTime = 2.0f;

	// 内部计时器，用于记录当前延迟了多久
	float CurrentBufferDelay = 0.0f;
};
