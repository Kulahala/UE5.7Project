#include "HUD/PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "AttributeComponent/AttributeComponent.h"

void UPlayerHUDWidget::SetHealthPercent(float Percent)
{
	if (PB_Health)
	{
		if (Percent < PB_Health->GetPercent())
		{
			CurrentBufferDelay = BufferDelayTime;
		}
		PB_Health->SetPercent(Percent);
	}
}

void UPlayerHUDWidget::BindToAttributes(UAttributeComponent* Attributes)
{
	if (Attributes)
	{
		Attributes->OnHealthChanged.AddDynamic(this, &UPlayerHUDWidget::SetHealthPercent);
		SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PB_Buffer && PB_Health)
	{
		const float BufferPercent = PB_Buffer->GetPercent();
		const float TargetPercent = PB_Health->GetPercent();

		if (BufferPercent > TargetPercent)
		{
			if (CurrentBufferDelay > 0.0f)
			{
				CurrentBufferDelay -= InDeltaTime;
			}
			else
			{
				const float NewPercent = FMath::FInterpTo(BufferPercent, TargetPercent, InDeltaTime, BufferInterpSpeed);
				PB_Buffer->SetPercent(NewPercent);
			}
		}
		else if (BufferPercent < TargetPercent)
		{
			PB_Buffer->SetPercent(TargetPercent);
		}
	}
}
