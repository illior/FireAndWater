// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Components/FAWInteractWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"

float UFAWInteractWidget::StartShow()
{
	StopAllAnimations();
	SetPercentage(0.0f);
	PlayAnimation(Show);

	return Show->GetEndTime();
}

float UFAWInteractWidget::StartHide()
{
	GetWorld()->GetTimerManager().ClearTimer(HoldTimer);

	if (IsAnimationPlaying(Show))
	{
		float CurrentAnimTime = GetAnimationCurrentTime(Show);

		StopAllAnimations();
		PlayAnimation(Hide, Hide->GetEndTime() - CurrentAnimTime);
		return CurrentAnimTime;
	}

	StopAllAnimations();
	PlayAnimation(Hide);

	return Hide->GetEndTime();
}

void UFAWInteractWidget::StartHold(float InHoldTime)
{
	SetPercentage(0.0f);
	StopAnimation(ProgressBarHide);
	ProgressBar->SetRenderOpacity(1.0f);

	HoldedTime = 0.0f;
	HoldTime = InHoldTime;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "UpdateHold");

	FTimerManagerTimerParameters Parameters;
	Parameters.bLoop = true;
	Parameters.bMaxOncePerFrame = true;

	GetWorld()->GetTimerManager().SetTimer(HoldTimer, Delegate, 0.005f, Parameters);
}

void UFAWInteractWidget::StopHold()
{
	GetWorld()->GetTimerManager().ClearTimer(HoldTimer);
	PlayAnimation(ProgressBarHide);
}

void UFAWInteractWidget::UpdateHold()
{
	HoldedTime += GetWorld()->GetDeltaSeconds();

	SetPercentage(FMath::Clamp(HoldedTime / HoldTime, 0.0f, 1.0f));
	if (HoldedTime >= HoldTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(HoldTimer);
	}
}

void UFAWInteractWidget::SetPercentage(float Percent)
{
	ReceiveSetPercentage(Percent);
}
