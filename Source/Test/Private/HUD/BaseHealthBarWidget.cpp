// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BaseHealthBarWidget.h"

#include "Components/ProgressBar.h"

void UBaseHealthBarWidget::SetHealthPercent(float Percent)
{
	if (PB_Health)
	{
		// 如果是扣血，重置延迟计时器
		if (Percent < PB_Health->GetPercent())
		{
			CurrentBufferDelay = BufferDelayTime;
		}
		PB_Health->SetPercent(Percent);
	}
}

void UBaseHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PB_Buffer && PB_Health)
	{
		const float BufferPercent = PB_Buffer->GetPercent();
		const float TargetPercent = PB_Health->GetPercent();

		// 如果缓冲条比真实血条高（被砍扣血）
		if (BufferPercent > TargetPercent)
		{
			// 如果还在延迟等待中，先倒计时
			if (CurrentBufferDelay > 0.0f)
			{
				CurrentBufferDelay -= InDeltaTime;
			}
			else
			{
				// 延迟结束，使用插值平滑下降
				const float NewPercent = FMath::FInterpTo(BufferPercent, TargetPercent, InDeltaTime, BufferInterpSpeed);
				PB_Buffer->SetPercent(NewPercent);
			}
		}
		// 如果真实血条反超（比如吃了回血药），缓冲条应该瞬间跟上，不需要动画
		else if (BufferPercent < TargetPercent)
		{
			PB_Buffer->SetPercent(TargetPercent);
		}
	}
}

